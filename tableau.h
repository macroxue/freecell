#ifndef TABLEAU_H
#define TABLEAU_H

#include "limits.h"
#include <assert.h>
#include <vector>
using namespace std;

#include "card.h"

class Tableau {
 public:
  Tableau() : index_(0), unsorted_size_(0), sorted_size_(0), stack_(0) {}
  Tableau(int index, int size) { set_cards(index, size); }

  void set_cards(int index, int size) {
    index_ = index;
    unsorted_size_ = size;
    sorted_size_ = 0;
    RecountSorted();
  }

  Card Top() const { return top_; }

  void Push(Card card) {
    assert(Accepting(card));
    assert(sorted_size_ < 13);
    if (sorted_size_++ > 0) PushTopToStack();
    top_ = card;
  }

  int Pop() {
    assert(sorted_size_ > 0);
    if (sorted_size_ > 1) {
      top_ = card(size() - 2);
      stack_ >>= 1;
      sorted_size_--;
      return 0;
    } else {
      sorted_size_--;
      return RecountSorted();
    }
  }

  int Move(Tableau* to, int super_move_size = INT_MAX) {
    auto count = CountMovable(*to);
    if (count > super_move_size) {
      assert(to->empty());
      count = super_move_size;
    }
    assert(0 < count && count <= sorted_size_);
    assert(to->Accepting(card(size() - count)));

    if (!to->empty()) to->PushTopToStack();
    if (count > 1) {
      to->stack_ =
          (to->stack_ << (count - 1)) | (stack_ & ((1 << (count - 1)) - 1));
    }
    to->top_ = top_;
    to->sorted_size_ += count;

    top_ = card(size() - count - 1);
    stack_ >>= count;
    sorted_size_ -= count;
    return RecountSorted();
  }

  int CountMovable(const Tableau& target) const {
    if (empty()) return 0;
    if (target.empty()) {
      return sorted_size_;
    } else {
      auto lead = target.Top();
      auto top = Top();
      int rank_diff = lead.rank() - top.rank();
      if (rank_diff <= 0) return 0;
      if (sorted_size_ < rank_diff) return 0;
      if ((rank_diff & 1) == (top.color() == lead.color())) return 0;
      return rank_diff;
    }
  }

  bool Accepting(Card card) const {
    return empty() ? true : card.IsBelow(Top());
  }

  bool empty() const { return !size(); }
  int size() const { return unsorted_size_ + sorted_size_; }
  int unsorted_size() const { return unsorted_size_; }
  int sorted_size() const { return sorted_size_; }

  Card card(int i) const {
    assert(i < size());
    if (i < unsorted_size_) return init_tableau_[index_][i];
    if (i == size() - 1) return top_;

    int depth = size() - 2 - i;
    bool is_major = stack_ & (1 << depth);
    int color = (depth & 1) ? top_.color() : !top_.color();
    int suit = is_major ? color : 3 - color;
    return Card(suit, top_.rank() + depth + 1);
  }

  int ChaosAt(int i) const {
    assert(0 < i && i <= index_);
    return std::max(0, init_tableau_[index_][i].rank() -
                           init_tableau_[index_][i - 1].rank());
  }

  int MeasureChaos() const {
    int chaos = 0;
    for (int i = 1; i < size(); ++i) {
      chaos += ChaosAt(i);
    }
    return chaos;
  }

  bool operator==(const Tableau& t) const {
    if (unsorted_size() != t.unsorted_size() ||
        sorted_size() != t.sorted_size())
      return false;
    if (sorted_size() == 0) return true;
    if (top_ != t.top_) return false;
    return stack_ == t.stack_;
    // unsigned mask = (1 << (sorted_size() - 1)) - 1;
    // return (stack_ & mask) == (t.stack_ & mask);
  }
  bool operator!=(const Tableau& t) const { return !(*this == t); }

  static Card init_tableau_[8][7];

 private:
  void PushTopToStack() { stack_ = (stack_ << 1) | top_.IsMajor(); }

  int RecountSorted() {
    if (sorted_size_ > 0 || unsorted_size_ == 0) return 0;

    top_ = card(size() - 1);
    stack_ = 0;
    int num_sorted = 1;
    while (num_sorted < size()) {
      int i = size() - num_sorted;
      if (!card(i).IsBelow(card(i - 1))) break;
      ++num_sorted;
    }

    for (int i = size() - num_sorted; i < size() - 1; ++i)
      stack_ = (stack_ << 1) | card(i).IsMajor();

    sorted_size_ += num_sorted;
    unsorted_size_ -= num_sorted;
    return num_sorted;
  }

  unsigned index_ : 4;
  unsigned unsorted_size_ : 4;
  unsigned sorted_size_ : 4;
  unsigned stack_ : 12;
  Card top_;
};

#endif
