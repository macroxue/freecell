#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <vector>
using namespace std;

enum MoveType {
  kNone,
  kTableauToFoundation,
  kTableauToReserve,
  kTableauToTableau,
  kReserveToFoundation,
  kReserveToTableau
};

struct Move {
  Move() : Move(kNone, 0, 0) {}
  Move(MoveType type, int from, int to = 0)
      : type(type), from(from), to(to) {}
  Move(char from, char to);

  string Encode() const;
  void Show() const;

  bool operator==(const Move& m) const {
    return type == m.type && from == m.from && to == m.to;
  }

  unsigned char type : 8;
  unsigned char from : 4;
  unsigned char to : 4;
};

string ReadSolution(int& seed);
vector<Move> DecodeSolution(const string& code);

#endif
