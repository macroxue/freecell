#include <assert.h>
#include <stdio.h>
#include "tableau.h"

void test_push_pop() {
  Tableau::init_tableau_[0][0] = Card(CLUB, KING);
  Tableau::init_tableau_[0][1] = Card(SPADE, RJ);
  Tableau::init_tableau_[0][2] = Card(HEART, R10);

  Tableau t(0, 3);
  assert(t.Top() == Card(HEART, R10));
  assert(t.size() == 3);
  assert(t.unsorted_size() == 1);

  assert(t.Accepting(Card(SPADE, R9)));
  assert(t.Accepting(Card(CLUB, R9)));
  assert(!t.Accepting(Card(HEART, R9)));
  assert(!t.Accepting(Card(DIAMOND, R9)));
  for (int suit = SPADE; suit <= CLUB; ++suit) {
    assert(!t.Accepting(Card(suit, R8)));
    assert(!t.Accepting(Card(suit, R8)));
    assert(!t.Accepting(Card(suit, R10)));
    assert(!t.Accepting(Card(suit, R10)));
  }

  t.Push(Card(CLUB, R9));
  assert(t.Top() == Card(CLUB, R9));
  assert(t.size() == 4);
  assert(t.unsorted_size() == 1);

  t.Pop();
  assert(t.Top() == Card(HEART, R10));
  assert(t.size() == 3);
  assert(t.unsorted_size() == 1);

  t.Pop();
  assert(t.Top() == Card(SPADE, RJ));
  assert(t.size() == 2);
  assert(t.unsorted_size() == 1);

  t.Pop();
  assert(t.Top() == Card(CLUB, KING));
  assert(t.size() == 1);
  assert(t.unsorted_size() == 0);

  t.Pop();
  assert(t.empty());
  assert(t.unsorted_size() == 0);

  for (int suit = SPADE; suit <= CLUB; ++suit) {
    for (int rank = ACE; rank <= KING; ++rank)
      assert(t.Accepting(Card(suit, rank)));
  }

  t.Push(Card(SPADE, KING));
  assert(t.Top() == Card(SPADE, KING));
  assert(t.size() == 1);
  assert(t.unsorted_size() == 0);
}

void test_move() {
  Tableau::init_tableau_[1][0] = Card(CLUB, KING);
  Tableau::init_tableau_[1][1] = Card(DIAMOND, RQ);
  Tableau::init_tableau_[1][2] = Card(HEART, R10);

  Tableau from(1, 3);
  from.Push(Card(CLUB, R9));

  Tableau::init_tableau_[2][0] = Card(SPADE, RJ);
  Tableau to(2, 1);
  from.Move(&to);

  assert(from.size() == 2);
  assert(from.unsorted_size() == 0);
  assert(to.size() == 3);
  assert(to.unsorted_size() == 0);

  to.Pop();
  to.Pop();
  to.Pop();
  assert(to.empty());

  auto card = from.Top();
  from.Pop();
  from.Push(card);

  from.Move(&to);
  assert(from.empty());
  assert(from.unsorted_size() == 0);
  assert(to.size() == 2);
  assert(to.unsorted_size() == 0);
}

void test_count_movable() {
  Tableau::init_tableau_[3][0] = Card(SPADE, ACE);
  Tableau::init_tableau_[3][1] = Card(HEART, KING);
  Tableau::init_tableau_[3][2] = Card(CLUB, R9);
  Tableau t(3, 3);
  t.Push(Card(DIAMOND, R8));
  t.Push(Card(SPADE, R7));
  assert(t.size() == 5);
  assert(t.unsorted_size() == 2);

  Tableau::init_tableau_[4][0] = Card(HEART, R9);
  assert(t.CountMovable(Tableau(4, 1)) == 0);
  Tableau::init_tableau_[4][0] = Card(HEART, R8);
  assert(t.CountMovable(Tableau(4, 1)) == 1);
  Tableau::init_tableau_[4][0] = Card(SPADE, R9);
  assert(t.CountMovable(Tableau(4, 1)) == 2);
  Tableau::init_tableau_[4][0] = Card(HEART, R10);
  assert(t.CountMovable(Tableau(4, 1)) == 3);
  Tableau::init_tableau_[4][0] = Card(HEART, RJ);
  assert(t.CountMovable(Tableau(4, 1)) == 0);
  Tableau::init_tableau_[4][0] = Card(CLUB, R7);
  assert(t.CountMovable(Tableau(4, 1)) == 0);
  Tableau::init_tableau_[4][0] = Card(CLUB, R6);
  assert(t.CountMovable(Tableau(4, 1)) == 0);
  assert(t.CountMovable(Tableau()) == 3);

  Tableau::init_tableau_[4][0] = Card(HEART, R10);
  Tableau target(4, 1);
  t.Move(&target);

  assert(t.CountMovable(Tableau()) == 1);
}

void test_chaos() {
  Tableau::init_tableau_[3][0] = Card(SPADE, ACE);
  Tableau::init_tableau_[3][1] = Card(HEART, KING);
  Tableau::init_tableau_[3][2] = Card(CLUB, R9);
  Tableau::init_tableau_[3][3] = Card(DIAMOND, RJ);
  Tableau t(3, 4);
  assert(t.ChaosAt(1) == 12);
  assert(t.ChaosAt(2) == 0);
  assert(t.ChaosAt(3) == 2);
  assert(t.MeasureChaos() == 14);
}

int main() {
  printf("sizeof(Tableau) = %lu\n", sizeof(Tableau));
  test_push_pop();
  test_move();
  test_count_movable();
  test_chaos();

  puts("PASSED");
  return 0;
}
