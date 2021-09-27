#ifndef FOUNDATION_H
#define FOUNDATION_H

#include <assert.h>

#include "card.h"

class Foundation {
 public:
  Foundation() : size_(0) {}

  Card Top(int suit) const { return Card(suit, size_ - 1); }

  void Push(Card card) {
    assert(Accepting(card));
    ++size_;
  }

  void Pop() {
    assert(!empty());
    --size_;
  }

  bool Accepting(Card card) const {
    return card.rank() == size_;
  }

  bool Has(Card card) const {
    return size_ >= 1 && card.rank() <= size_ - 1;
  }

  bool empty() const { return !size(); }
  int size() const { return size_; }

 private:
  char size_;
};

#endif
