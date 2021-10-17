#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>

#include "card.h"

template <class T, int N>
class Array {
 public:
  int max_size() const { return N; }
  int size() const { return size_; }
  bool empty() const { return size_ == 0; }

  T operator[](int index) const {
    assert(index < size_);
    return items_[index];
  }

  bool operator==(const Array& r) const {
    if (size() != r.size()) return false;
    for (int i = 0; i < size_; ++i) {
      if ((*this)[i] != r[i]) return false;
    }
    return true;
  }

  void push_back(const T& c) {
    assert(size_ < N);
    items_[size_++] = c;
  }

  void erase(int index) {
    assert(index < size_);
    --size_;
    for (int i = index; i < size_; ++i) {
      items_[i] = items_[i + 1];
    }
  }

 private:
  T items_[N];
  unsigned char size_ = 0;
};

#endif
