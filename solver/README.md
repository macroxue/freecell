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

With safe auto moves:
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

Given enough resources, it was able to find a solution of 76 moves for the
infamous 94717719 game with maximum auto moves.
```
$ ./solver -A -n128 94717719 20
...
        Moves: 76  Estimate: 0  Auto: 52  Cost: 0  Unsorted: 0
94717719:6r565r52135157785r65b64r7r727874b71267626r64a6761rc71r141312c1723231273r343232b35r856r86b88356858r8184c835651528241r216r263r2341464342a4454r27812r2r2825
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
```
![](https://github.com/macroxue/freecell/blob/master/solver/visualizer.png)
