#ifndef _BUCKET_H
#define _BUCKET_H

#include <functional>
#include <vector>
using namespace std;

#include "node.h"

class Bucket {
 public:
  Bucket(int num_bins);

  void Clear();
  void Add(Node *node, int index);
  void Remove(Node *node, int index);
  Node *RemoveMin();
  Node *RemoveMax();

  void Iterate(std::function<void(Node *node)> work) const;

  int min() const { return min_; }
  int max() const { return max_; }
  int size() const { return size_; }
  bool empty() const { return !size(); }

  int lowerbound() const { return lowerbound_; }
  void set_lowerbound(int lowerbound) { lowerbound_ = lowerbound; }

 private:
  void FindNewMin();
  void FindNewMax();

  vector<Node *> bins_;
  int min_;
  int max_;
  int size_;

  int lowerbound_ = 0;
};

Bucket::Bucket(int num_bins) : bins_(num_bins) { Clear(); }

void Bucket::Clear() {
  for (auto &bin : bins_) bin = nullptr;
  min_ = bins_.size() + 1;
  max_ = -1;
  size_ = 0;
}

void Bucket::Add(Node *node, int index) {
  node->after_ = bins_[index];
  bins_[index] = node;
  if (!node->after_) {
    // first node in a bins_
    if (min_ > index) min_ = index;
    if (max_ < index) max_ = index;
  }
  size_++;
}

// Note: this function is not tested
void Bucket::Remove(Node *node, int index) {
  if (bins_[index] == node) {
    bins_[index] = node->after_;
    if (bins_[index] == nullptr) {
      if (size_ > 1) {
        if (index == min_) FindNewMin();
        if (index == max_) FindNewMax();
      } else {
        min_ = bins_.size() + 1;
        max_ = -1;
      }
    }
  } else {
    for (auto cursor = bins_[index]; cursor; cursor = cursor->after_) {
      if (cursor->after_ == node) {
        cursor->after_ = node->after_;
        break;
      }
    }
  }
  size_--;
}

Node *Bucket::RemoveMin() {
  auto *node = bins_[min_];
  bins_[min_] = node->after_;
  if (!node->after_) FindNewMin();
  size_--;
  return node;
}

Node *Bucket::RemoveMax() {
  auto *node = bins_[max_];
  bins_[max_] = node->after_;
  if (!node->after_) FindNewMax();
  size_--;
  return node;
}

void Bucket::Iterate(std::function<void(Node *node)> work) const {
  if (empty()) return;
  for (int index = min_; index <= max_; ++index) {
    for (auto cursor = bins_[index]; cursor;) {
      auto next = cursor->after_;
      work(cursor);
      cursor = next;
    }
  }
}

void Bucket::FindNewMin() {
  while (min_ < max_ && bins_[min_] == nullptr) min_++;
}

void Bucket::FindNewMax() {
  while (min_ < max_ && bins_[max_] == nullptr) max_--;
}

#endif
