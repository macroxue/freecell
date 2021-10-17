#include "node.h"
#include <assert.h>
#include <stdio.h>

void test_play() {
  Node n(2);
  n.Show();
  n.TableauToFoundation(0);
  n.TableauToReserve(1);
  n.TableauToFoundation(1);
  n.TableauToTableau(1, 2);
  n.TableauToTableau(5, 2);
  n.TableauToTableau(5, 3);
  n.TableauToReserve(0);
  n.TableauToFoundation(0);
  n.TableauToTableau(0, 2);
  n.TableauToTableau(1, 3);
  n.TableauToTableau(0, 1);
  n.TableauToReserve(5);
  n.ReserveToTableau(1, 5);
  n.TableauToTableau(5, 4);
  n.TableauToTableau(5, 2);
  n.TableauToTableau(5, 4);
  n.TableauToReserve(0);
  n.TableauToTableau(1, 0);
  n.ReserveToTableau(0, 2);
  n.ReserveToTableau(0, 2);
  n.TableauToTableau(2, 0);
  n.Show();
}

void test_duplicate() {
  Node m(2);
  Node n(2);
  assert(m == n);

  m.TableauToReserve(5);
  assert(m != n);
  m.TableauToTableau(5, 3);
  assert(m != n);
  m.TableauToFoundation(0);
  assert(m != n);
  m.Show();

  n.TableauToFoundation(0);
  assert(m != n);
  n.TableauToReserve(5);
  assert(m != n);
  n.TableauToTableau(5, 3);
  assert(m == n);
  n.Show();
}

int main() {
  printf("sizeof(Node) = %lu\n", sizeof(Node));
  printf("sizeof(CompressedMoves) = %lu\n", sizeof(Node::CompressedMoves));
  printf("sizeof(Move) = %lu\n", sizeof(Move));

  test_duplicate();
  test_play();

  puts("PASSED");
  return 0;
}
