#include "move.h"
#include "node.h"

Move::Move(char from, char to) {
  if ('0' <= from && from <= '7') {
    if (to == 'h')
      *this = Move(kTableauToFoundation, from - '0');
    else if (to == 'r')
      *this = Move(kTableauToReserve, from - '0');
    else if ('0' <= to && to <= '7')
      *this = Move(kTableauToTableau, from - '0', to - '0');
    else
      assert(!"Unknown encoding");
  } else if ('a' <= from && from < 'h') {
    if (to == 'h')
      *this = Move(kReserveToFoundation, from - 'a');
    else if ('0' <= to && to <= '7')
      *this = Move(kReserveToTableau, from - 'a', to - '0');
    else
      assert(!"Unknown encoding");
  }
}

string Move::Encode() const {
  switch (type) {
    case kTableauToFoundation:
      return string("") + char('0' + from) + char('h');
    case kTableauToReserve:
      return string("") + char('0' + from) + char('r');
    case kTableauToTableau:
      return string("") + char('0' + from) + char('0' + to);
    case kReserveToFoundation:
      return string("") + char('a' + from) + char('h');
    case kReserveToTableau:
      return string("") + char('a' + from) + char('0' + to);
    default:
      return "";
  }
}

void Move::Show() const {
  switch (type) {
    case kTableauToFoundation:
      return (void)printf("==> tableau %d to foundation", from);
    case kTableauToReserve:
      return (void)printf("==> tableau %d to reserve", from);
    case kTableauToTableau:
      return (void)printf("==> tableau %d to tableau %d", from, to);
    case kReserveToFoundation:
      return (void)printf("==> reserve %d to foundation", from);
    case kReserveToTableau:
      return (void)printf("==> reserve %d to tableau %d", from, to);
    default:
      return;
  }
}

string ReadSolution(int seed) {
  int solution_seed;
  char solution_code[1024];
  int items = scanf("%d:%s", &solution_seed, solution_code);
  if (items != 2) {
    puts("failed to read solution");
    exit(1);
  }
  assert(solution_seed == seed);
  return solution_code;
}

vector<Move> DecodeSolution(const string& code) {
  vector<Move> moves;
  for (int i = 0; i < code.size(); ++i) {
    moves.push_back(Move(code[i], code[i + 1]));
    ++i;
  }
  return moves;
}
