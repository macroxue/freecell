#include "move.h"
#include "node.h"

Move::Move(char from, char to) {
  if ('1' <= from && from <= '8') {
    if (to == 'h')
      *this = Move(kTableauToFoundation, from - '1');
    else if (to == 'r')
      *this = Move(kTableauToReserve, from - '1');
    else if ('1' <= to && to <= '8')
      *this = Move(kTableauToTableau, from - '1', to - '1');
    else
      assert(!"Unknown encoding");
  } else if ('a' <= from && from < 'h') {
    if (to == 'h')
      *this = Move(kReserveToFoundation, from - 'a');
    else if ('1' <= to && to <= '8')
      *this = Move(kReserveToTableau, from - 'a', to - '1');
    else
      assert(!"Unknown encoding");
  }
}

string Move::Encode() const {
  switch (type) {
    case kTableauToFoundation:
      return string("") + char('1' + from) + char('h');
    case kTableauToReserve:
      return string("") + char('1' + from) + char('r');
    case kTableauToTableau:
      return string("") + char('1' + from) + char('1' + to);
    case kReserveToFoundation:
      return string("") + char('a' + from) + char('h');
    case kReserveToTableau:
      return string("") + char('a' + from) + char('1' + to);
    default:
      return "";
  }
}

void Move::Show() const {
  switch (type) {
    case kTableauToFoundation:
      return (void)printf("==> tableau %c to foundation", '1' + from);
    case kTableauToReserve:
      return (void)printf("==> tableau %c to reserve", '1' + from);
    case kTableauToTableau:
      return (void)printf("==> tableau %c to tableau %c", '1' + from, '1' + to);
    case kReserveToFoundation:
      return (void)printf("==> reserve %c to foundation", 'a' + from);
    case kReserveToTableau:
      return (void)printf("==> reserve %c to tableau %c", 'a' + from, '1' + to);
    default:
      return;
  }
}

string ReadSolution(int& solution_seed) {
  char solution_code[1024];
  int items = scanf("%d:%s", &solution_seed, solution_code);
  if (items != 2) {
    puts("failed to read solution");
    exit(1);
  }
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
