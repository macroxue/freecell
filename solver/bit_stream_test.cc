#include <stdio.h>
#include "bit_stream.h"

int main() {
  BitStream<1000> b;

  assert(b.BitsNeededFor(1) == 1);
  assert(b.BitsNeededFor(2) == 1);
  assert(b.BitsNeededFor(3) == 2);
  assert(b.BitsNeededFor(4) == 2);
  assert(b.BitsNeededFor(5) == 3);
  assert(b.BitsNeededFor(8) == 3);
  assert(b.BitsNeededFor(9) == 4);

  for (int i = 0; i < 100; ++i) {
    b.Write(i, i + 1);
  }
  BitStream<1000>::Reader r(b);
  for (int i = 0; i < 100; ++i) {
    int v = r.Read(i + 1);
    assert(v == i);
  }
  printf("Copy size = %d\n", b.copy_size());
  return 0;
}

