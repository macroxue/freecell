#ifndef STOCK_H
#define STOCK_H

#include <assert.h>
#include <vector>
using namespace std;

#include "card.h"

static constexpr int kInitStockCards = 65;

class Stock {
 public:
  Stock() {}
  Stock(int size)
      : init_size_(size), stock_size_(size), waste_size_(0), top_index_(0) {
    *mutable_waste_card_map() = 0;
  }

  Card Top() const {
    assert(waste_size_ > 0);
    return init_stock_[top_index_];
  }

  void Pop() {
    assert(waste_size_ > 0);
    --waste_size_;
    if (waste_size_ > 0) {
      top_index_ = 63 - __builtin_clzll(waste_card_map());
      *mutable_waste_card_map() &= ~(1ULL << top_index_);
    }
  }

  void Flip() {
    assert(size() > 0);
    if (waste_size_ > 0) {
      *mutable_waste_card_map() |= (1ULL << top_index_);
    }
    top_index_ = init_size_ - stock_size_;
    ++waste_size_;
    --stock_size_;
  }

  bool empty() const { return !size(); }
  int size() const { return stock_size_ + waste_size_; }
  int stock_size() const { return stock_size_; }
  int waste_size() const { return waste_size_; }

  bool operator==(const Stock& s) const {
    return stock_size_ == s.stock_size_ && waste_size_ == s.waste_size_ &&
           (waste_size_ == 0 || waste_card_map() == s.waste_card_map());
  }
  bool operator!=(const Stock& s) const { return !(*this == s); }

  void Show() const {
    printf("Stock: ");
    for (int i = init_size_ - 1; i >= init_size_ - stock_size_; --i)
      printf("%s ", init_stock_[i].ToString());
    puts("");
    printf("Waste: ");
    if (waste_size_ > 0) {
      printf("%s ", Top().ToString());
      for (int i = 63; i >= 0; --i) {
        if (waste_card_map() & (1ULL << i))
          printf("%s ", init_stock_[i].ToString());
      }
    }
    puts("");
  }

  static Card init_stock_[kInitStockCards];

  static void set_init_stock(const vector<Card>& cards) {
    for (int i = 0; i < cards.size(); ++i)
      init_stock_[cards.size() - 1 - i] = cards[i];
  }

  unsigned long long* mutable_waste_card_map() {
    return (unsigned long long*)waste_card_map_;
  }

  unsigned long long waste_card_map() const {
    return *(unsigned long long*)waste_card_map_;
  }

 private:
  unsigned waste_card_map_[2];
  char init_size_;
  char stock_size_;
  char waste_size_;
  char top_index_;
};

#endif
