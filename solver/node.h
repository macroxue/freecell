#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>
using namespace std;

#include "array.h"
#include "bit_stream.h"
#include "foundation.h"
#include "list.h"
#include "move.h"
#include "options.h"
#include "tableau.h"

static constexpr int kTotalCards = 52;
static constexpr int kMinMoves = 0;
static constexpr int kMaxMoves = 128;

class Node;

class Pool {
 public:
  Node* New();
  Node* New(const Node& node);
  void Delete(Node* node);

 private:
  Node* Allocate();
  void Free(Node* node);

  Node* head_ = nullptr;
  vector<unique_ptr<Node[]>> chunks_;
};

class Node {
 public:
  using CompressedMoves = BitStream<512>;
  using Reserve = Array<Card, 4>;

  Node() {}
  Node(int deal_num) {
    vector<Card> cards(kTotalCards);
    for (int i = 0; i < kTotalCards; ++i) cards[i] = Card(i);

    // Generate Microsoft deal.
    uint64_t seed = deal_num;
    for (int i = 0; i < cards.size(); i++) {
      int cards_left = cards.size() - i;
      seed = seed * 214013L + 2531011L;
      uint32_t rand = (seed >> 16) & 0x7fff;
      int pos = deal_num < 0x80000000 ? rand % cards_left
                                      : (rand | 0x8000) % cards_left;
      swap(cards[pos], cards[cards_left - 1]);
    }

    reverse(cards.begin(), cards.end());
    set_cards(cards);
    AutoPlay();
  }

  void set_cards(const vector<Card>& cards) {
    assert(cards.size() == kTotalCards);

    for (int i = 0; i < cards.size(); ++i) {
      int col = i % 8;
      int row = i / 8;
      Tableau::init_tableau_[col][row] = cards[i];
    }
    for (int i = 0; i < 8; ++i) {
      tableau_[i].set_cards(i, i < 4 ? 7 : 6);
    }

    cards_unsorted_ = 0;
    for (int i = 0; i < 8; ++i) {
      cards_unsorted_ += tableau_[i].unsorted_size();
    }

    moves_performed_ = 0;
    moves_estimated_ = kTotalCards;
  }

  Node(const Node& node) {
    int copy_size = reinterpret_cast<char*>(&moves_) + node.moves_.copy_size() -
                    reinterpret_cast<char*>(this);
    memcpy(this, &node, copy_size);
  }

  int MeasureTableauChaos() const {
    int chaos = 0;
    for (int t = 0; t < 8; ++t) chaos += tableau_[t].MeasureChaos();
    return chaos;
  }

  MoveType last_move_type() const {
    return static_cast<MoveType>(last_auto_move_.type == kNone ?
                                 last_move_.type : last_auto_move_.type);
  }
  int last_move_from() const {
    return last_auto_move_.type == kNone ? last_move_.from : last_auto_move_.from;
  }
  int last_move_to() const {
    return last_auto_move_.type == kNone ? last_move_.to : last_auto_move_.to;
  }

  /* Move priorites:
     reserve -> foundation
     tableau -> foundation
     reserve -> tableau
     tableau -> tableau
     tableau -> reserve

   Triangle moves to avoid:
    * tableau -> tableau/reserve -> foundation  should be  tableau -> foundation
    * reserve -> tableau -> foundation          should be  reserve -> foundation
    * tableau -> reserve -> tableau             should be  tableau -> tableau
    * tableau -> tableau -> reserve             should be  tableau -> reserve

   Move matrix:
   r1->f1  allows        allows       allows        allows         allows
           r2->f2        all t->f     all r->t      all t->t       all t->r
           if f1 <= f2
   t1->f1  allows        allows       allows        allows         allows
           r->f1         t2->f2       all r->t      all t->t       all t->r
                         if f1 == f2
                         or t1 <= t2
   r1->t1  disallows     disallows    allows        allows         allows
           all r->f      all t->f     r2->t2        multi t1->t2   t2->r2
                                      if t1 == t2   or t2->t3      if t1!=t2
                                      or r1 <= r2
   t1->t2  disallows     allows       allows        allows         allows
           all r->f      t1->f        r->t1         some t->t      t3->r
                                      or r->t2                     if t3!=t2
   t1->r1  disallows     allows       allows        allows         allows
           all r->f      t1->f        r->t1         t1->t2         t2->r2
                                      disallows     or t2->t1      if t1<=t2
                                      r1->t
  */
  bool AllowReserveToFoundation() const {
    if (options.max_auto_play) return false;
    return last_move_type() != kReserveToTableau &&
           last_move_type() != kTableauToTableau &&
           last_move_type() != kTableauToReserve;
  }
  bool AllowReserveToFoundation(int f) const {
    if (last_move_type() == kReserveToFoundation) return last_move_to() <= f;
    if (last_move_type() == kTableauToFoundation) return last_move_to() == f;
    return true;
  }
  bool AllowTableauToFoundation() const {
    if (options.max_auto_play) return false;
    return last_move_type() != kReserveToTableau;
  }
  bool AllowTableauToFoundation(int t, int f) const {
    if (last_move_type() == kTableauToFoundation)
      return last_move_to() == f || last_move_from() <= t;
    if (last_move_type() == kTableauToTableau) return last_move_from() == t;
    if (last_move_type() == kTableauToReserve) return last_move_from() == t;
    return true;
  }
  bool AllowReserveToTableau(int r, int t) const {
    if (last_move_type() == kReserveToTableau)
      return last_move_to() == t || last_move_from() <= r;
    if (last_move_type() == kTableauToTableau)
      return t == last_move_from() || t == last_move_to();
    if (last_move_type() == kTableauToReserve)
      return t == last_move_from() || r != last_move_to();
    return true;
  }
  bool AllowTableauToTableau(int t1, int t2) const {
    for (int i = 0; i < forbidden_moves_.size(); ++i) {
      if (forbidden_moves_[i].from == t1 && forbidden_moves_[i].to == t2)
        return false;
    }
    if (last_move_type() == kTableauToReserve)
      return last_move_from() == t1 || last_move_from() == t2;
    return true;
  }
  bool AllowTableauToReserve(int t) const {
    if (last_move_type() == kReserveToTableau) return last_move_to() != t;
    if (last_move_type() == kTableauToTableau) return last_move_to() != t;
    if (last_move_type() == kTableauToReserve) return last_move_from() <= t;
    return true;
  }

  List<Node> Expand(Pool* pool) const {
    List<Node> new_nodes;
    for (int r = 0; r < reserve_.size(); ++r) {
      const auto& card = reserve_[r];
      if (AllowReserveToFoundation()) {
        auto f = FindFoundation(card);
        if (f >= 0 && AllowReserveToFoundation(f)) {
          new_nodes.Append(pool->New(*this)->ReserveToFoundation(r)->AutoPlay());
        }
      }
      // only need to try one empty tableau if any.
      bool tried_empty_tableau = false;
      for (int i = 0; i < 8; ++i) {
        if (tableau_[i].Accepting(card) && AllowReserveToTableau(r, i)) {
          if (!tableau_[i].empty() || !tried_empty_tableau) {
            new_nodes.Append(pool->New(*this)->ReserveToTableau(r, i)->AutoPlay());
            if (tableau_[i].empty()) tried_empty_tableau = true;
          }
        }
      }
    }
    for (int i = 0; i < 8; ++i) {
      if (tableau_[i].empty()) continue;

      if (AllowTableauToFoundation()) {
        auto card = tableau_[i].Top();
        auto f = FindFoundation(card);
        if (f >= 0 && AllowTableauToFoundation(i, f))
          new_nodes.Append(pool->New(*this)->TableauToFoundation(i)->AutoPlay());
      }

      bool tried_empty_tableau = false;
      for (int j = 0; j < 8; ++j) {
        if (j == i) continue;
        if (!AllowTableauToTableau(i, j)) continue;

        auto count = tableau_[i].CountMovable(tableau_[j]);
        if (count > 0) {
          // Don't move a fully sorted stack to empty tableau.
          if (tableau_[j].empty() && tableau_[i].unsorted_size() == 0 &&
              tableau_[i].sorted_size() <= MaxSuperMoveSize(i, j))
            continue;
          // Only need to try one empty tableau if any.
          if (tableau_[j].empty() || count <= MaxSuperMoveSize(i, j)) {
            if (tableau_[j].empty()) {
              if (tried_empty_tableau) continue;
              else tried_empty_tableau = true;
            }
            new_nodes.Append(pool->New(*this)->TableauToTableau(i, j)->AutoPlay());
          }
        }
      }

      if (reserve_.size() < reserve_.max_size() && AllowTableauToReserve(i)) {
        new_nodes.Append(pool->New(*this)->TableauToReserve(i)->AutoPlay());
      }
    }

    EncodeMoves(new_nodes);
    return new_nodes;
  }

  bool CanAutoPlay(Card card) {
    if (!options.auto_play && !options.max_auto_play) return false;

    if (!foundation_[card.suit()].Accepting(card)) return false;
    if (options.max_auto_play) return true;
    if (card.rank() <= R2) return true;
    if (card.color() == RED) {
      return foundation_[SPADE].Has(Card(SPADE, card.rank() - 1))
          && foundation_[CLUB].Has(Card(CLUB, card.rank() - 1));
    } else {
      return foundation_[HEART].Has(Card(HEART, card.rank() - 1))
          && foundation_[DIAMOND].Has(Card(DIAMOND, card.rank() - 1));
    }
  }

  Node* AutoPlay() {
    last_auto_move_.type = kNone;
    if (!options.auto_play && !options.max_auto_play) return this;

    unsigned old_auto_plays;
    do {
      old_auto_plays = auto_plays;
      for (int i = 0; i < reserve_.size(); ++i) {
        if (CanAutoPlay(reserve_[i])) {
          ReserveToFoundation(i, true);
          ++auto_plays;
        }
      }
      for (int i = 0; i < 8; ++i) {
        if (!tableau_[i].empty() && CanAutoPlay(tableau_[i].Top())) {
          TableauToFoundation(i, true);
          ++auto_plays;
        }
      }
    } while (auto_plays > old_auto_plays);
    return this;
  }

  int MaxSuperMoveSize(int from, int to) const {
    int free_cells = reserve_.max_size() - reserve_.size();
    int empty_tableaus = 0;
    for (int t = 0; t < 8; ++t) {
      if (t == from || t == to) continue;
      if (tableau_[t].empty()) ++empty_tableaus;
    }
    return (free_cells + 1) << empty_tableaus;
  }

  int FindReserve() const { return reserve_.size(); }

  int FindFoundation(Card card) const {
    auto suit = card.suit();
    if (foundation_[suit].Accepting(card)) return suit;
    return -1;
  }

  Node* ReserveToFoundation(int r, bool auto_play = false) {
    Card card = reserve_[r];
    reserve_.erase(r);
    foundation_[card.suit()].Push(card);
    moves_estimated_ -= 1;
    if (auto_play) {
      last_auto_move_ = Move(kReserveToFoundation, r, card.suit());
    } else {
      last_move_ = Move(kReserveToFoundation, r, card.suit());
      moves_performed_++;
    }
    CheckInvariant();
    return this;
  }

  Node* ReserveToTableau(int r, int t) {
    Card card = reserve_[r];
    reserve_.erase(r);
    tableau_[t].Push(card);
    moves_estimated_ -= 0;
    last_move_ = Move(kReserveToTableau, r, t);
    moves_performed_++;
    LiftMoveRestriction(t);
    CheckInvariant();
    return this;
  }

  Node* TableauToFoundation(int t, bool auto_play = false) {
    auto card = tableau_[t].Top();
    int new_sorted = tableau_[t].Pop();
    foundation_[card.suit()].Push(card);
    cards_unsorted_ -= new_sorted;
    moves_estimated_ -= 1;
    if (auto_play) {
      last_auto_move_ = Move(kTableauToFoundation, t, card.suit());
    } else {
      last_move_ = Move(kTableauToFoundation, t, card.suit());
      moves_performed_++;
    }
    LiftMoveRestriction(t);
    CheckInvariant();
    return this;
  }

  Node* TableauToReserve(int t) {
    auto card = tableau_[t].Top();
    int new_sorted = tableau_[t].Pop();
    reserve_.push_back(card);
    cards_unsorted_ -= new_sorted;
    moves_estimated_ -= 0;
    last_move_ = Move(kTableauToReserve, t, reserve_.size() - 1);
    moves_performed_++;
    LiftMoveRestriction(t);
    CheckInvariant();
    return this;
  }

  Node* TableauToTableau(int s, int t) {
    int new_sorted = tableau_[s].Move(&tableau_[t], MaxSuperMoveSize(s, t));
    cards_unsorted_ -= new_sorted;
    moves_estimated_ -= 0;
    last_move_ = Move(kTableauToTableau, s, t);
    moves_performed_++;
    LiftMoveRestriction(s);
    LiftMoveRestriction(t);
    if (new_sorted == 0) AddMoveRestriction(t, s);
    CheckInvariant();
    return this;
  }

  void AddMoveRestriction(int from, int to) {
    forbidden_moves_.push_back({(char)from, (char)to});
  }

  void LiftMoveRestriction(int t) {
    for (int i = 0; i < forbidden_moves_.size(); ++i) {
      if (forbidden_moves_[i].from == t || forbidden_moves_[i].to == t)
        forbidden_moves_.erase(i--);
    }
  }

  void CheckInvariant() const {
#ifdef DEBUG
    set<Card> cards;
    auto in_foundation = [this](const Card& card) -> bool {
      const auto& f = foundation_[card.suit()];
      return !f.empty() && card.rank() <= f.Top(card.suit()).rank();
    };
    for (int r = 0; r < reserve_.size(); ++r) {
      auto card = reserve_[r];
      assert(!in_foundation(card));
      assert(cards.find(card) == cards.end());
      cards.insert(card);
    }
    for (int t = 0; t < 8; ++t) {
      for (int i = 0; i < tableau_[t].size(); ++i) {
        auto card = tableau_[t].card(i);
        assert(!in_foundation(card));
        assert(cards.find(card) == cards.end());
        cards.insert(card);
      }
    }
    for (int i = 0; i < kTotalCards; ++i) {
      Card card(i);
      if (!in_foundation(card)) assert(cards.find(card) != cards.end());
    }
#endif
  }

  string CompleteSolution();

  void EncodeMoves(const List<Node>& new_nodes) const {
    int index = 0;
    for (auto* node : new_nodes) node->moves_.Write(index++, new_nodes.size());
  }

  void PlayMoves(const vector<Move>& moves);
  void ShowSummary() const;
  void Show(const Move& next_move = Move()) const;

  const Reserve& reserve() const { return reserve_; }
  const Foundation& foundation(int f) const { return foundation_[f]; }
  const Tableau& tableau(int t) const { return tableau_[t]; }
  const CompressedMoves& moves() const { return moves_; }
  const Move& last_move() const { return last_move_; }

  int bin() const { return cost(); }
  int min_total_moves() const { return moves_performed_ + moves_estimated_; }
  int moves_performed() const { return moves_performed_; }
  void set_moves_performed(int moves) { moves_performed_ = moves; }
  int moves_estimated() const { return moves_estimated_; }
  int cost() const {
    int color_diff = 0;
    if (options.minimize_color_diff) {
      color_diff = abs(foundation_[SPADE].size() - foundation_[HEART].size() -
                       foundation_[DIAMOND].size() + foundation_[CLUB].size());
    }
    return auto_plays + moves_estimated_ + cards_unsorted_ + reserve_.size() + color_diff;
  }
  int cards_unsorted() const { return cards_unsorted_; }

  bool operator==(const Node& n) const {
    if (reserve_.size() != n.reserve_.size()) return false;

    for (int f = 0; f < 4; ++f)
      if (foundation_[f].size() != n.foundation_[f].size()) return false;

    for (int t = 0; t < 8; ++t)
      if (tableau_[t].sorted_size() != n.tableau_[t].sorted_size() ||
          tableau_[t].size() != n.tableau_[t].size())
        return false;

    for (int t = 0; t < 8; ++t)
      if (tableau_[t] != n.tableau_[t]) return false;

    return true;
  }
  bool operator!=(const Node& n) const { return !(*this == n); }

  unsigned hash() const { return hash_; }

  void ComputeHash() {
    hash_ = 0;
    for (int i = 0; i < reserve_.size(); ++i) {
      hash_ += reserve_rand_[reserve_[i].card()];
    }
    for (int i = 0; i < 8; ++i) {
      hash_ += tableau_unsorted_rand_[i][tableau_[i].unsorted_size()];
      hash_ += tableau_sorted_rand_[i][tableau_[i].sorted_size()];
      if (tableau_[i].sorted_size())
        hash_ += tableau_top_rand_[i][tableau_[i].Top().card()];
    }
  }

  static const Node& goal() { return goal_; }
  static void Initialize();
  static void InitializeHashRand(int count, vector<unsigned>* rand);

#if 0
  static void* operator new(size_t size) {
    if (pool_) {
      auto node = pool_;
      pool_ = *reinterpret_cast<Node**>(pool_);
      return node;
    } else {
      return ::operator new(size);
    }
  }

  static void operator delete(void* ptr) {
    *reinterpret_cast<Node**>(ptr) = pool_;
    pool_ = static_cast<Node*>(ptr);
  }
#endif

 private:
  Foundation foundation_[4];
  Tableau tableau_[8];
  Reserve reserve_;

  unsigned char cards_unsorted_ = 0;
  unsigned char moves_performed_ = 0;
  unsigned char moves_estimated_ = 0;
  unsigned char auto_plays = 0;
  unsigned hash_ = 0;

  struct M {
    char from : 4;
    char to : 4;
  };
  Array<M, 4> forbidden_moves_;
  Move last_move_;
  Move last_auto_move_;
  // Must be the last field before link pointers for copy to work.
  CompressedMoves moves_;

  friend class HashTable;
  Node* prev_ = nullptr;
  Node* next_ = nullptr;

  friend class Pool;
  friend class List<Node>;
  friend class Bucket;
  Node* after_ = nullptr;

  static vector<unsigned> reserve_rand_;
  static vector<vector<unsigned>> tableau_unsorted_rand_;
  static vector<vector<unsigned>> tableau_sorted_rand_;
  static vector<vector<unsigned>> tableau_top_rand_;

  static Node* pool_;
  static Node goal_;
};

// TODO(hanhong): Can't return a ScopedNode!
class ScopedNode {
 public:
  ScopedNode(Pool* pool) : pool_(pool), node_(nullptr) {}
  ScopedNode(Pool* pool, Node* node) : pool_(pool), node_(node) {}
  ~ScopedNode() { reset(); }

  Node* operator->() const { return node_; }
  const Node& operator*() const { return *node_; }
  operator bool() const { return node_ != nullptr; }

  void reset() { pool_->Delete(node_); }
  void reset(Node* node) {
    reset();
    node_ = node;
  }
  Node* release() {
    auto old_node = node_;
    node_ = nullptr;
    return old_node;
  }

 private:
  Pool* const pool_;
  Node* node_;
};

#endif
