#include "deals.h"
#include "node.h"

int main(int argc, char* argv[]) {
  int seed = 1;
  if (argc > 1) seed = atoi(argv[1]);

  Node layout(seed);
  if (seed < 0) layout.set_cards(GetDeal(-seed));

  auto moves = DecodeSolution(ReadSolution(seed));
  for (const auto& move : moves) {
    Node previous(layout);
    layout.PlayMoves({move});
    previous.Show(layout.last_move());
  }
  layout.Show();
  return 0;
}
