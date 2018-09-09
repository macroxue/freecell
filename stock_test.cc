#include <assert.h>
#include <stdio.h>
#include "stock.h"

void short_test() {
  vector<Card> cards{Card(HEART, ACE), Card(SPADE, KING), Card(CLUB, R2)};
  Stock::set_init_stock(cards);
  Stock s(cards.size());
  assert(s.stock_size() == 3);
  assert(s.waste_size() == 0);

  for (int i = 0; i < 1; ++i) {
    s.Flip();
    assert(s.stock_size() == 2);
    assert(s.waste_size() == 1);
    assert(s.Top() == Card(CLUB, R2));

    s.Flip();
    assert(s.stock_size() == 1);
    assert(s.waste_size() == 2);
    assert(s.Top() == Card(SPADE, KING));

    s.Flip();
    assert(s.stock_size() == 0);
    assert(s.waste_size() == 3);
    assert(s.Top() == Card(HEART, ACE));
  }

  s.Pop();
  assert(s.stock_size() == 0);
  assert(s.waste_size() == 2);
  assert(s.Top() == Card(SPADE, KING));

  s.Pop();
  assert(s.stock_size() == 0);
  assert(s.waste_size() == 1);
  assert(s.Top() == Card(CLUB, R2));

  s.Pop();
  assert(s.stock_size() == 0);
  assert(s.waste_size() == 0);
}

void flip_all_test() {
  vector<Card> cards;
  for (int i = 0; i < kInitStockCards; ++i)
    cards.push_back(Card(i % kInitStockCards));
  Stock::set_init_stock(cards);

  Stock s(cards.size());
  assert(s.size() == kInitStockCards);
  assert(s.stock_size() == kInitStockCards);
  assert(s.waste_size() == 0);

  for (int i = 0; i < kInitStockCards; ++i) {
    s.Flip();
    assert(s.Top() == cards[kInitStockCards - 1 - i]);
  }
  for (int i = 0; i < kInitStockCards; ++i) {
    assert(s.Top() == cards[i]);
    s.Pop();
  }
  assert(s.empty());
}

int main() {
  printf("sizeof(Stock) = %lu\n", sizeof(Stock));

  short_test();
  flip_all_test();

  puts("PASSED");
  return 0;
}
