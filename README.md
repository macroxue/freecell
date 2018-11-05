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
./solver -a <game#> <beam size>
```

Without auto moves:
```
$ ./solver 617
...
        Moves: 87  Estimate: 0  Auto: 0  Cost: 0  Unsorted: 0
617:3r351r1661313r3r3h6hch3r63d30r050h656h727h72c763101rc171767h16171h3h3h5h3h212h2r232hahbh2r2h5h5h5h4r4h5h5h535h5hbh0h050h46424h010h3h4h2h7h6h6h6hch1h3h5h5h6h6hbh1h1h1h5h5h6hah
...
```

With auto moves:
```
$ ./solver -a 617
...
        Moves: 43  Estimate: 0  Auto: 48  Cost: 0  Unsorted: 0
617:727r721r107617070115616r6r60b03r3r3763b636313h5h057073131ra121272r2627474h43424h020701
...
```

### Notations
* 01234567: Tableau 
* abcd: Freecell/Reserve
* r: Freecell/Reserve
* h: Home/Foundation

Examples:
* 72: from Tableau 7 to Tableau 2
* 7r: from Tableau 7 to Reserve 
* b0: from Reserve 1 to Tableau 0
* 3h: from Tableau 3 to Foundation

## Visualize
There is also a visualizer program which takes a solution from stdin and visualizes its moves like below.
```
$ ./visualizer -a 617
617:727r721r107617070115616r6r60b03r3r3763b636313h5h057073131ra121272r2627474h43424h020701
        Moves: 0  Estimate: 52  Auto: 0  Cost: 96  Unsorted: 44                         
Reserve: [ ]  Foundation: [S ] [H ] [D ] [C ]         
Tableau: 0   1   2   3   4   5   6   7                                                  
        7H  AH  5S  3C  5C  8S  2H  AD                                                   
        TH  7C  QH  AS  6H  8D  AC  KD                                                 
        TD  QS  3D  9H  6C  8H  3H  TS                                                  
        KH  5D  9C  3S  8C  7D  4H  JC                
        4S  QC  9S  9D  7S  6D  2S  2C                                                  
        4C  TC  2D  5H  JS  6S  JD  QD                 
        JH  KC  KS  4D              ↙ 
                ↑                                                                       
==> tableau 7 to tableau 2      Moves: 1  Estimate: 52  Auto: 0  Cost: 95  Unsorted: 43
Reserve: [ ]↖  Foundation: [S ] [H ] [D ] [C ]                                           
Tableau: 0   1   2   3   4   5   6   7                 
        7H  AH  5S  3C  5C  8S  2H  AD
        TH  7C  QH  AS  6H  8D  AC  KD                                                   
        TD  QS  3D  9H  6C  8H  3H  TS                    
        KH  5D  9C  3S  8C  7D  4H  JC                                                 
        4S  QC  9S  9D  7S  6D  2S  2C                                                  
        4C  TC  2D  5H  JS  6S  JD  ↖                  
        JH  KC  KS  4D                                                                  
                QD                                                                        
...
```
