#ifndef CARD_H
#define CARD_H

#include <stdio.h>

enum Color { BLACK, RED };
enum Suit { SPADE, HEART, DIAMOND, CLUB };
enum Rank { ACE, R2, R3, R4, R5, R6, R7, R8, R9, R10, RJ, RQ, KING };

class Card {
 public:
  Card() {}
  Card(const Card& card) : card_(card.card_) {}
  Card(int card) : card_(card) {}
  Card(int suit, int rank) : card_(suit + (rank << 2)) {}

  int card() const { return card_; }
  int suit() const { return card_ & 3; }
  int rank() const { return card_ >> 2; }
  int color() const { return suit() == SPADE || suit() == CLUB ? BLACK : RED; }

  bool IsMajor() const { return suit() == SPADE || suit() == HEART; }

  bool IsAbove(Card foundation_top) const {
    return suit() == foundation_top.suit() &&
           rank() == foundation_top.rank() + 1;
  }
  bool IsBelow(Card tableau_top) const {
    return color() != tableau_top.color() && rank() + 1 == tableau_top.rank();
  }

  bool operator<(const Card& card) const { return card_ < card.card_; }
  bool operator==(const Card& card) const { return card_ == card.card_; }
  bool operator!=(const Card& card) const { return !(*this == card); }

  char* ToString() const {
    static char str[32];
    sprintf(str, "\e[3%dm%c%c\e[0m", 2 - color(), "A23456789TJQK"[rank()],
            "SHDC"[suit()]);
    return str;
  }

 private:
  char card_ = -1;
};

#endif
