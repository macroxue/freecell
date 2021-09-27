#include <assert.h>
#include <stdio.h>
#include "foundation.h"

int main() {
  printf("sizeof(Foundation) = %lu\n", sizeof(Foundation));
  Foundation f;
  assert(f.size() == 0);
  assert(f.empty());

  for (int rank = ACE; rank <= KING; ++rank) {
    assert(f.Accepting(Card(CLUB, rank)));
    for (int r = ACE; r <= KING; ++r)
      assert(r == rank || !f.Accepting(Card(CLUB, r)));

    f.Push(Card(CLUB, rank));
    assert(f.Top(CLUB) == Card(CLUB, rank));
  }
  for (int rank = KING; rank >= ACE; --rank) {
    assert(f.Top(CLUB) == Card(CLUB, rank));
    f.Pop();
    assert(f.Accepting(Card(CLUB, rank)));
    for (int r = ACE; r <= KING; ++r)
      assert(r == rank || !f.Accepting(Card(CLUB, r)));
  }
  assert(f.size() == 0);
  assert(f.empty());

  puts("PASSED");
  return 0;
}
