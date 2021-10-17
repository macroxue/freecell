#include "deals.h"
#include "node.h"
#include "options.h"

int main(int argc, char* argv[]) {
  options = Options(argc, argv);

  auto solution = ReadSolution(options.seed);
  auto foundation_moves = std::count(solution.begin(), solution.end(), 'h');
  options.max_auto_play = (foundation_moves == 0);
  options.auto_play = (foundation_moves < 52);

  Node layout(options.seed);
  if (options.seed < 0) layout.set_cards(GetDeal(-options.seed));

  auto moves = DecodeSolution(solution);
  for (const auto& move : moves) {
    Node previous(layout);
    if (previous.cards_unsorted() == 0) break;
    layout.PlayMoves({move});
    previous.Show(layout.last_move());
  }
  layout.Show();
  return 0;
}
