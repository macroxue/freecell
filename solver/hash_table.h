#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <assert.h>

#include "node.h"

class HashTable {
 public:
  HashTable(int num_bins) : HashTable(num_bins, false) {}

  HashTable(int num_bins, bool exact)
      : exact_(exact), bins_(num_bins), bin_mask_(num_bins - 1) {
    // number of bins must be power of two
    assert((num_bins & (num_bins - 1)) == 0);
    Clear();
  }

  ~HashTable() {
#ifdef DEBUG
    const double M = 1e6;
    printf("items %.1fm  lookups %.1fm  hits %.1fm  rate %.3f\n",
           num_additions_ / M, num_lookups_ / M, num_hits_ / M,
           double(num_hits_) / num_lookups_);
#endif
  }

  void Clear() {
    for (auto& bin : bins_) bin = nullptr;
    size_ = 0;
  }

  void Show(int level) const {
    int non_empty_bins = 0;
    int max_count = 0;
    const Node* max_bin = nullptr;
    for (const auto& bin : bins_) {
      non_empty_bins += (bin != nullptr);

      int count = 0;
      for (auto cursor = bin; cursor; cursor = cursor->next_) ++count;
      if (max_count < count) {
        max_count = count;
        max_bin = bin;
      }
    }
    printf("%d: elements %d total bins %lu  non-empty %d max conflicts %d\n",
           level, size(), bins_.size(), non_empty_bins, max_count);

    static int show_times = 0;
    if (++show_times == 100) {
      for (auto cursor = max_bin; cursor; cursor = cursor->next_)
        cursor->Show();
    }
  }

  Node* Find(const Node* node) const {
    ++num_lookups_;
    auto index = node->hash() & bin_mask_;
    for (auto cursor = bins_[index]; cursor; cursor = cursor->next_) {
      // Aggressively consider nodes to be the same when they look similar.
      // Strict comparison uses (... && *node == *cursor).
      if (node->hash() == cursor->hash()) {
        if ((!exact_ && node->reserve() == cursor->reserve()) ||
            (exact_ && *node == *cursor)) {
          ++num_hits_;
          return cursor;
        }
      }
    }
    return nullptr;
  }

  void Add(Node* node) {
    ++num_additions_;
    auto index = node->hash() & bin_mask_;
    auto* next = bins_[index];
    node->next_ = next;
    node->prev_ = nullptr;
    if (next) next->prev_ = node;
    bins_[index] = node;
    ++size_;
  }

  void Remove(Node* node) {
    ++num_removals_;
    auto* prev = node->prev_;
    auto* next = node->next_;
    if (prev)
      prev->next_ = next;
    else
      bins_[node->hash() & bin_mask_] = next;
    if (next) next->prev_ = prev;
    --size_;
  }

  bool empty() const { return !size_; }
  int size() const { return size_; }

 private:
  const bool exact_;
  vector<Node*> bins_;
  int bin_mask_;
  int size_ = 0;

  mutable long num_lookups_ = 0;
  mutable long num_hits_ = 0;
  long num_additions_ = 0;
  long num_removals_ = 0;
};

#endif
