#include <vector>
using namespace std;

#include "card.h"
#include "node.h"

const char* deals[] = {
    "4C 9S TC 4H 6S 7C AC TD "
    "3H JC AH 6D KS 6C KC 3D "
    "2D 9D 6H 8C 3S JH 4D 8S "
    "AS 9H JS QD 2C QC KD TS "
    "JD 7D QS 5D 2S AD 5C 9C "
    "8H 7H 3C QH 5H TH 4S 5S "
    "KH 2H 8D 7S",
    "AC AH 3D JC 9C 7C JS 8C "
    "9D 6C TC AS KS 7S 4S TS "
    "5S 4H AD QS 3C 6D QC 4C "
    "9S 6S 2D TH QD 3S 8H 5H "
    "3H JD 2S QH 9H 5D 8S 2C "
    "5C 6H 8D 2H KC 7D JH KD "
    "TD 4D KH 7H",
    "QC 2H 5C 6H 7H 5D 8C 6D "
    "QS KD AD AS TD 7S KC TH "
    "7D 4D JH 5H 5S 7C 4H AH "
    "3H AC 2S 9S 9C 2D JD 8D "
    "3D 8H 2C 9D QD 6C KS QH "
    "8S 3C TC KH 4C JS 6S TS "
    "4S 3S 9H JC",
};

vector<Card> GetDeal(int seed) {
  const char* deal = deals[seed - 1];
  vector<Card> init_cards;
  vector<bool> presented(kTotalCards, false);
  for (int i = 0; i < kTotalCards; ++i) {
    int rank = ACE;
    switch (deal[i * 3]) {
      case 'A':
        rank = ACE;
        break;
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        rank = deal[i * 3] - '1';
        break;
      case 'T':
        rank = R10;
        break;
      case 'J':
        rank = RJ;
        break;
      case 'Q':
        rank = RQ;
        break;
      case 'K':
        rank = KING;
        break;
      default:
        printf("bad rank: %s\n", deal + i * 3);
        assert(!"bad rank");
    }
    int suit = SPADE;
    switch (deal[i * 3 + 1]) {
      case 'S':
        suit = SPADE;
        break;
      case 'H':
        suit = HEART;
        break;
      case 'D':
        suit = DIAMOND;
        break;
      case 'C':
        suit = CLUB;
        break;
      default:
        printf("bad suit: %s\n", deal + i * 3 + 1);
        assert(!"bad suit");
    }
    auto card = Card(suit, rank);
    if (presented[card.card()]) {
      printf("Duplicate card: %s\n", card.ToString());
      assert(!"Duplicate card");
    }
    init_cards.push_back(card);
    presented[card.card()] = true;
  }

  assert(init_cards.size() == kTotalCards);
  return init_cards;
}

