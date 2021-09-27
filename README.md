# Freecell solver, supporting Microsoft game numbers

The solver is modified after [Triple Klondike Solver](https://github.com/macroxue/triple-klondike).
It supports two ways of counting moves: with and without auto moves to foundations.

## Build
```
make
```

## Solve
Usage:
```
./solver -[aA] -n<threads> <game#> <beam size>
```

Without auto moves:
```
$ ./solver 617
...
        Moves: 87  Estimate: 0  Auto: 0  Cost: 0  Unsorted: 0
617:4r462r2772424r4r4h7hch4r74d41r161h767h838h83c874212rc282878h27282h4h4h6h4h323h3r343hahbh3r3h6h6h6h5r5h6h6h646h6hbh1h131h57565h121h4h5h6h8h7h7h7hch2h4h7h7hbh3h3h2h2h2h3h3h7hah
...
```

With auto moves:
```
$ ./solver -a 617
...
        Moves: 41  Estimate: 0  Auto: 48  Cost: 0  Unsorted: 0
617:2r4r4627724r4r42dh4r74d41r161h768383c874212rc28287272ra232383r3438585h57535h131812
...
```

With maximum auto moves:
```
$ ./solver -A 617
...
        Moves: 35  Estimate: 0  Auto: 52  Cost: 0  Unsorted: 0
617:4r462r2772424r4r4r74d41r7174831683c8212rc2828727285254575r353r38136412
...
```

### Notations
* 12345678: Tableau
* abcd: Freecell/Reserve
* r: Freecell/Reserve
* h: Home/Foundation

Examples:
* 72: from Tableau 7 to Tableau 2
* 7r: from Tableau 7 to Reserve
* b1: from Reserve b to Tableau 1
* 3h: from Tableau 3 to Foundation

## Visualize
There is also a visualizer program which takes a solution from stdin and visualizes its moves like below.
```
$ ./visualizer
617:2r4r4627724r4r42dh4r74d41r161h768383c874212rc28287272ra232383r3438585h57535h131812
        Moves: 0  Estimate: 52  Auto: 0  Cost: 96  Unsorted: 44
Reserve: [ ]↖  Foundation: [S ] [H ] [D ] [C ]
Tableau: 1   2   3   4   5   6   7   8
        7H  AH  5S  3C  5C  8S  2H  AD
        TH  7C  QH  AS  6H  8D  AC  KD
        TD  QS  3D  9H  6C  8H  3H  TS
        KH  5D  9C  3S  8C  7D  4H  JC
        4S  QC  9S  9D  7S  6D  2S  2C
        4C  TC  2D  5H  JS  6S  JD  QD
        JH  KC  KS  4D
            ↖
==> tableau 2 to reserve        Moves: 1  Estimate: 52  Auto: 0  Cost: 96  Unsorted: 43
Reserve: [ KC ]↖  Foundation: [S ] [H ] [D ] [C ]
Tableau: 1   2   3   4   5   6   7   8
        7H  AH  5S  3C  5C  8S  2H  AD
        TH  7C  QH  AS  6H  8D  AC  KD
        TD  QS  3D  9H  6C  8H  3H  TS
        KH  5D  9C  3S  8C  7D  4H  JC
        4S  QC  9S  9D  7S  6D  2S  2C
        4C  TC  2D  5H  JS  6S  JD  QD
        JH      KS  4D
                    ↖
==> tableau 4 to reserve        Moves: 2  Estimate: 52  Auto: 0  Cost: 96  Unsorted: 42
Reserve: [ KC 4D ]  Foundation: [S ] [H ] [D ] [C ]
Tableau: 1   2   3   4   5   6   7   8
        7H  AH  5S  3C  5C  8S  2H  AD
        TH  7C  QH  AS  6H  8D  AC  KD
        TD  QS  3D  9H  6C  8H  3H  TS
        KH  5D  9C  3S  8C  7D  4H  JC
        4S  QC  9S  9D  7S  6D  2S  2C
        4C  TC  2D  5H  JS  6S  JD  QD
        JH      KS  ↘       ↑
...
```

# Statistics
Below are statistics of the first 2000 games, with `./solver -A -n8 <game#> 18`.
Among the 2000, the only unsolvable game using maximum auto moves is 1941, which
is solvable in 36 moves using safe auto moves. The average number of moves for
the solvable games is 25.54.

![](https://github.com/macroxue/freecell/blob/master/statistics.2k.png)


 | Moves | # Games |
 |-------|---------|
 | 13    | 1       |
 | 14    | 1       |
 | 15    | 5       |
 | 16    | 12      |
 | 17    | 22      |
 | 18    | 36      |
 | 19    | 47      |
 | 20    | 69      |
 | 21    | 107     |
 | 22    | 132     |
 | 23    | 178     |
 | 24    | 188     |
 | 25    | 205     |
 | 26    | 205     |
 | 27    | 172     |
 | 28    | 157     |
 | 29    | 144     |
 | 30    | 112     |
 | 31    | 79      |
 | 32    | 55      |
 | 33    | 33      |
 | 34    | 15      |
 | 35    | 12      |
 | 36    | 5       |
 | 37    | 5       |
 | 40    | 1       |
 | 41    | 1       |
