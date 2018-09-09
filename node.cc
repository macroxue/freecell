#include "node.h"

Node* Node::pool_;
Node Node::goal_;

vector<unsigned> Node::reserve_rand_;
vector<vector<unsigned>> Node::tableau_unsorted_rand_;
vector<vector<unsigned>> Node::tableau_sorted_rand_;
vector<vector<unsigned>> Node::tableau_top_rand_;

void Node::Initialize() {
  InitializeHashRand(kTotalCards, &reserve_rand_);

  tableau_unsorted_rand_.resize(8);
  tableau_sorted_rand_.resize(8);
  tableau_top_rand_.resize(8);
  for (int i = 0; i < 8; ++i) {
    InitializeHashRand(8, &tableau_unsorted_rand_[i]);
    InitializeHashRand(14, &tableau_sorted_rand_[i]);
    InitializeHashRand(kTotalCards, &tableau_top_rand_[i]);
  }

  pool_ = nullptr;

  for (int i = 0; i < 4; ++i) {
    for (int rank = ACE; rank <= KING; ++rank)
      goal_.foundation_[i].Push(Card(i, rank));
  }
  goal_.ComputeHash();
}

void Node::InitializeHashRand(int count, vector<unsigned>* rand) {
  for (int i = 0; i < count; ++i) rand->push_back(lrand48());
}

void Node::ShowSummary() const {
  printf("\tMoves: %d  Estimate: %d  Cost: %d  Hash: %u Unsorted: %d\n",
         moves_performed_, moves_estimated_, cost(), hash_, cards_unsorted_);
}

void Node::Show() const {
  last_move_.Show();
  ShowSummary();
  printf("Reserve: [ ");
  for (int i = 0; i < reserve_.size(); ++i) {
    printf("%s ", reserve_[i].ToString());
  }
  printf("]  Foundation: ");
  for (int i = 0; i < 4; ++i)
    if (foundation_[i].empty())
      printf("[%c ] ", "SHDC"[i]);
    else
      printf("[%s] ", foundation_[i].Top(i).ToString());
  puts("");
  for (int i = 0; i < 8; ++i) {
    printf("Tableau %2d: ", i);
    for (int j = 0; j < tableau_[i].unsorted_size(); ++j)
      printf("%s ", tableau_[i].card(j).ToString());
    printf("| ");
    for (int j = tableau_[i].unsorted_size(); j < tableau_[i].size(); ++j)
      printf("%s ", tableau_[i].card(j).ToString());
    puts("");
  }
}

void Node::Show(const Move& next_move) const {
  last_move_.Show();
  ShowSummary();
  printf("Reserve: [");
  for (int i = 0; i < reserve_.size(); ++i) {
    printf(" %s", reserve_[i].ToString());
    if ((next_move.type == kReserveToFoundation ||
         next_move.type == kReserveToTableau) &&
        next_move.from == i)
      printf("==>");
  }
  printf(" ] ");
  if (next_move.type == kTableauToReserve) printf("<==");
  printf("  Foundation: ");
  for (int i = 0; i < 4; ++i) {
    if ((next_move.type == kTableauToFoundation ||
         next_move.type == kReserveToFoundation) &&
        next_move.to == i)
      printf("==>");
    if (foundation_[i].empty())
      printf("[%c ] ", "SHDC"[i]);
    else
      printf("[%s] ", foundation_[i].Top(i).ToString());
  }
  puts("");
  for (int i = 0; i < 8; ++i) {
    printf("Tableau %2d: ", i);
    for (int j = 0; j < tableau_[i].unsorted_size(); ++j)
      printf("%s ", tableau_[i].card(j).ToString());
    printf("| ");
    for (int j = tableau_[i].unsorted_size(); j < tableau_[i].size(); ++j)
      printf("%s ", tableau_[i].card(j).ToString());
    if ((next_move.type == kTableauToFoundation ||
         next_move.type == kTableauToReserve ||
         next_move.type == kTableauToTableau) &&
        next_move.from == i)
      printf("==>");
    if ((next_move.type == kReserveToTableau ||
         next_move.type == kTableauToTableau) &&
        next_move.to == i)
      printf("<==");
    puts("");
  }
}

void Node::PlayMoves(const vector<Move>& moves) {
  for (const auto& move : moves) {
    switch (move.type) {
      case kNone:
        break;
      case kReserveToFoundation:
        ReserveToFoundation(move.from);
        break;
      case kReserveToTableau:
        ReserveToTableau(move.from, move.to);
        break;
      case kTableauToFoundation:
        TableauToFoundation(move.from);
        break;
      case kTableauToReserve:
        TableauToReserve(move.from);
        break;
      case kTableauToTableau:
        TableauToTableau(move.from, move.to);
    }
  }
}

string Node::CompleteSolution() {
  assert(cards_unsorted_ == 0);
  string code;
  bool all_tableau_empty;
  bool progress;
  do {
    progress = false;
    for (int i = 0; i < reserve_.size(); ++i) {
      int f = FindFoundation(reserve_[i]);
      if (f >= 0) {
        ReserveToFoundation(i);
        code += last_move_.Encode();
        --i;
        progress = true;
      }
    }
    all_tableau_empty = true;
    for (int i = 0; i < 8; ++i) {
      while (!tableau_[i].empty()) {
        int f = FindFoundation(tableau_[i].Top());
        if (f >= 0) {
          TableauToFoundation(i);
          code += last_move_.Encode();
          progress = true;
        } else
          break;
      }
      all_tableau_empty &= tableau_[i].empty();
    }
    if (!progress) {
      Show();
      assert(!"No progress completing solution");
    }
  } while (!all_tableau_empty || !reserve_.empty());
  return code;
}

Node* Pool::New(const Node& node) { return new (Allocate()) Node(node); }
Node* Pool::New() { return new (Allocate()) Node(); }
void Pool::Delete(Node* node) { Free(node); }

Node* Pool::Allocate() {
  if (!head_) {
    auto nodes = new Node[256];
    for (int i = 0; i < 256; ++i) Free(&nodes[i]);
  }
  auto old_head = head_;
  head_ = head_->after_;
  return old_head;
}

void Pool::Free(Node* node) {
  if (node) {
    node->after_ = head_;
    head_ = node;
  }
}
