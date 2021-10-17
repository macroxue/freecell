#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
using namespace std;

#include "bucket.h"
#include "deals.h"
#include "hash_table.h"
#include "node.h"
#include "options.h"

class Beam {
 public:
  Beam(int seed, int beam_size, int beam_id, int num_beams);
  void PrefixSearch(const Node& layout, vector<Move> moves);
  void SubmitWork(List<Node>* new_work);

 private:
  Node* CreateNewLevel(const Bucket& cur_level, Bucket* new_level);
  Node* BeamSearch(const Node& layout);
  string EncodeSolution(const Node& start, const Node& finish) const;

  List<Node> GetWork();
  Node* ProcessNewNodes(List<Node> new_nodes, Bucket* new_level);
  int TargetBeam(unsigned hash) const {
    // Shift bits so hash table can be better used.
    return (hash + (hash >> 24)) % num_beams_;
  }

  void EnterBarrier();
  bool BarrierDone();
  void Barrier();
  bool AllBeamsEmpty(int level) const;

  const int seed_;
  const int beam_size_;
  const int beam_id_;
  const int num_beams_;

  int upperbound_ = kMaxMoves + 1;
  vector<Bucket> levels_;
  std::unique_ptr<HashTable> hash_table_;
  Node shared_solution_;  // to be shared with other beams
  mutable Pool pool_;

  int sequence_number_;
  std::atomic<int> barrier_;

  List<Node> work_;
  std::mutex mu_;
};

vector<std::unique_ptr<Beam>> beams;

Beam::Beam(int seed, int beam_size, int beam_id, int num_beams)
    : seed_(seed),
      beam_size_(beam_size),
      beam_id_(beam_id),
      num_beams_(num_beams),
      sequence_number_(0),
      barrier_(0) {
  const int kNumBins = (kMaxMoves - kMinMoves) * 2;
  for (int i = 0; i < kMaxMoves; ++i) levels_.emplace_back(Bucket(kNumBins));
  hash_table_.reset(new HashTable(beam_size_ * 2));
}

void Beam::SubmitWork(List<Node>* new_work) {
  if (new_work->empty()) return;
  mu_.lock();
  work_.Append(new_work);
  mu_.unlock();
}

List<Node> Beam::GetWork() {
  List<Node> new_work;
  mu_.lock();
  new_work.Append(&work_);
  mu_.unlock();
  return new_work;
}

void Beam::EnterBarrier() {
  sequence_number_ = !sequence_number_;
  if (beam_id_ != 0) barrier_ = sequence_number_;
}

bool Beam::BarrierDone() {
  if (beam_id_ == 0) {
    for (int i = 1; i < num_beams_; ++i)
      if (beams[i]->barrier_ != sequence_number_) return false;
    beams[0]->barrier_ = sequence_number_;
    return true;
  } else
    return (beams[0]->barrier_ == sequence_number_);
}

void Beam::Barrier() {
  EnterBarrier();
  while (!BarrierDone()) sched_yield();
}

bool Beam::AllBeamsEmpty(int level) const {
  for (int i = 0; i < num_beams_; ++i)
    if (beams[i]->levels_[level].size() > 0) return false;
  return true;
}

Node* Beam::CreateNewLevel(const Bucket& cur_level, Bucket* new_level) {
  vector<List<Node>> partitions(num_beams_);
  ScopedNode solution(&pool_);

  auto process_new_solution = [&](Node* new_solution) {
    if (!new_solution) return;
    solution.reset(new_solution);
    if (num_beams_ == 1) return;

    // If solution is produced by this beam, send it to other beams to lower
    // their upperbounds.
    if (TargetBeam(solution->hash()) == beam_id_) {
      for (int i = 0; i < num_beams_; ++i) {
        if (i == beam_id_) continue;
        partitions[i].Append(pool_.New(*solution));
      }
    }
  };

  int expand_count = 0;
  cur_level.Iterate([&](Node* node) {
    if (node->moves_performed() >= upperbound_ - 1) return;
    auto new_nodes = node->Expand(&pool_);
    if (new_nodes.empty()) return;

    if (num_beams_ == 1) {
      for (auto* node : new_nodes) node->ComputeHash();
      process_new_solution(ProcessNewNodes(new_nodes, new_level));
    } else {
      for (auto* node : new_nodes) {
        node->ComputeHash();
        partitions[TargetBeam(node->hash())].Append(node);
      }
      if (++expand_count < 100) return;
      expand_count = 0;
      for (int i = 0; i < num_beams_; ++i) beams[i]->SubmitWork(&partitions[i]);
      process_new_solution(ProcessNewNodes(GetWork(), new_level));
    }
  });
  if (num_beams_ > 1) {
    for (int i = 0; i < num_beams_; ++i) beams[i]->SubmitWork(&partitions[i]);
    EnterBarrier();
    while (!BarrierDone())
      process_new_solution(ProcessNewNodes(GetWork(), new_level));
    for (int round = 0; round < 2; ++round) {
      for (int i = 0; i < num_beams_; ++i) beams[i]->SubmitWork(&partitions[i]);
      Barrier();
      process_new_solution(ProcessNewNodes(GetWork(), new_level));
    }
    assert(work_.empty());
    for (int i = 0; i < num_beams_; ++i) assert(partitions[i].empty());
    Barrier();
  }

  // hash_table_->Show(beam_id_);
  // cur_level.Iterate([&](Node* node) { hash_table_->Remove(node); });
  return solution.release();
}

Node* Beam::ProcessNewNodes(List<Node> new_nodes, Bucket* new_level) {
  ScopedNode solution(&pool_);
  for (auto* new_node : new_nodes) {
    if (new_node->min_total_moves() >= upperbound_ ||
        new_node->bin() < new_level->lowerbound()) {
      pool_.Delete(new_node);
      continue;
    }
    if (new_node->cards_unsorted() == 0 &&
        new_node->min_total_moves() < upperbound_) {
      solution.reset(new_node);
      upperbound_ = solution->min_total_moves();
      continue;
    }

    if ((new_level->size() == beam_size_ &&
         new_node->bin() > new_level->max()) ||
        hash_table_->Find(new_node)) {
      pool_.Delete(new_node);
    } else if (new_level->size() < beam_size_) {
      new_level->Add(new_node, new_node->bin());
      hash_table_->Add(new_node);
    } else {
      auto max_node = new_level->RemoveMax();
      hash_table_->Remove(max_node);
      pool_.Delete(max_node);
      new_level->Add(new_node, new_node->bin());
      hash_table_->Add(new_node);
    }
  }
  return solution.release();
}

Node* Beam::BeamSearch(const Node& layout) {
  auto root = pool_.New(layout);
  root->ComputeHash();
  levels_[0].Add(root, root->bin());
  hash_table_->Add(root);

  ScopedNode solution(&pool_);
  int max_level_size = 0;
  for (int i = 0; i < kMaxMoves; ++i) {
    if (num_beams_ == 1) {
      if (levels_[i].empty()) break;
    } else {
      Barrier();
      if (AllBeamsEmpty(i)) break;
      Barrier();
    }
    if (beam_id_ == 0) {
      char progress[30];
      sprintf(progress, "%s%4d %8d", string('\b', 13).c_str(), i,
              levels_[i].size());
      printf("%s", progress);
      fflush(stdout);
      max_level_size = max(max_level_size, levels_[i].size());
    }
    auto new_solution = CreateNewLevel(levels_[i], &levels_[i + 1]);
    if (new_solution) solution.reset(new_solution);
    constexpr int kPreservedLevels = 1;
    if (i >= kPreservedLevels) {
      bool first = true;
      levels_[i - kPreservedLevels].Iterate([&](Node* node) {
#if 0
        if (i == 75 && first) {
          first = false;
          node->Show();
          auto code = EncodeSolution(layout, *node);
          puts(code.c_str());
        }
#endif
        hash_table_->Remove(node);
        pool_.Delete(node);
      });
      levels_[i - kPreservedLevels].Clear();
    }
  }
  for (auto& level : levels_) {
    level.Iterate([&](Node* node) {
      hash_table_->Remove(node);
      pool_.Delete(node);
    });
    level.Clear();
  }
  if (beam_id_ == 0) {
    printf("%s%8d\n", string('\b', 8).c_str(), max_level_size);
  }
  return solution.release();
}

string Beam::EncodeSolution(const Node& start, const Node& finish) const {
  string code;
  ScopedNode node(&pool_, pool_.New(start));
  Node::CompressedMoves::Reader reader(finish.moves());
  for (int i = 0; i < finish.moves_performed(); ++i) {
    if (node->cards_unsorted() == 0) {
      code += node->CompleteSolution();
      break;
    }

    auto new_nodes = node->Expand(&pool_).ToVector();
    int move_index = reader.Read(new_nodes.size());
    assert(move_index < new_nodes.size());
    auto picked_node = new_nodes[move_index];
    for (auto* new_node : new_nodes)
      if (new_node != picked_node) pool_.Delete(new_node);
    node.reset(picked_node);
    code += node->last_move().Encode();
  }
  return code;
}

void Beam::PrefixSearch(const Node& layout, vector<Move> moves) {
  upperbound_ = moves.empty() ? kMaxMoves : moves.size();
  if (beam_id_ == 0) printf("upperbound %d\n", upperbound_);
  int start = 0;
  bool solved = false;
  do {
    ScopedNode prefix(&pool_, pool_.New(layout));
    prefix->PlayMoves({&moves[0], &moves[start]});

    if (beam_id_ == 0) printf("%d              ", start);
    ScopedNode solution(&pool_, BeamSearch(*prefix));
    if (solution) {
      solved = true;
      string code;
      for (int i = 0; i < start; ++i) code += moves[i].Encode();

      if (num_beams_ > 1) {
        // Use the same one solution in case different ones are found.
        if (beam_id_ == 0) new (&shared_solution_) Node(*solution);
        Barrier();
        if (beam_id_ != 0) solution.reset(new Node(beams[0]->shared_solution_));
        Barrier();
      }

      solution->CompleteSolution();
      code += EncodeSolution(*prefix, *solution);
      moves = DecodeSolution(code);
      if (beam_id_ == 0) {
        solution->ShowSummary();
        printf("%d:", seed_);
        puts(code.c_str());
      }
    }
    start += options.auto_play ? 10 : 20;
  } while (start < moves.size());
  if (!solved) puts("No solution");
}

int main(int argc, char* argv[]) {
  options = Options(argc, argv);

  Node::Initialize();

  Node layout(options.seed);
  if (options.deal > 0) layout.set_cards(GetDeal(options.deal));
  layout.Show();

  string code;
  if (ftell(stdin) != -1) code = ReadSolution(options.seed);
  auto moves = DecodeSolution(code);

  for (int i = 0; i < options.num_beams; ++i)
    beams.emplace_back(
        new Beam(options.seed, options.beam_size, i, options.num_beams));

  vector<std::unique_ptr<std::thread>> threads;
  for (int i = 0; i < options.num_beams; ++i)
    threads.emplace_back(new std::thread(
        std::bind(&Beam::PrefixSearch, beams[i].get(), layout, moves)));
  for (int i = 0; i < options.num_beams; ++i) threads[i]->join();

  return 0;
}
