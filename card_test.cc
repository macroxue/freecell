#include <assert.h>
#include <stdio.h>
#include "card.h"

int main() {
  for (int suit = SPADE; suit <= CLUB; ++suit) {
     for (int rank = ACE; rank <= KING ; ++rank) {
       Card c(suit, rank);
       assert(c.suit() == suit);
       assert(c.rank() == rank);
     }
  }
  assert(Card(SPADE, KING).color() == BLACK);
  assert(Card(HEART, KING).color() == RED);
  assert(Card(DIAMOND, KING).color() == RED);
  assert(Card(CLUB, KING).color() == BLACK);

  assert(Card(SPADE, KING).IsMajor());
  assert(Card(HEART, KING).IsMajor());
  assert(!Card(DIAMOND, KING).IsMajor());
  assert(!Card(CLUB, KING).IsMajor());

  assert(Card(SPADE, R2).IsAbove(Card(SPADE, ACE)));
  assert(!Card(SPADE, R3).IsAbove(Card(SPADE, ACE)));
  assert(!Card(HEART, R2).IsAbove(Card(SPADE, ACE)));

  assert(Card(SPADE, ACE).IsBelow(Card(HEART, R2)));
  assert(Card(SPADE, ACE).IsBelow(Card(DIAMOND, R2)));
  assert(!Card(SPADE, ACE).IsBelow(Card(SPADE, R2)));
  assert(!Card(SPADE, ACE).IsBelow(Card(CLUB, R2)));

  puts("PASSED");
  return 0;
}
