#!/bin/bash

url="https://macroxue.github.io/freecell/game/freecell.html?deal="
echo "## Games solvable with max auto play"
echo
echo "| Moves | Games |"
echo "|-------|-------|"
for n in $(seq 5 10); do
  echo "|$n|" $(grep -h -E ":.{$((n*2))}$" block.* | grep -v h | cut -d: -f1 | sort -n) "|" |\
    sed -e "s% \([0-9][0-9]*\)% [\1](${url}\1)%g"
done | grep freecell
echo "|...|...|"
for n in $(seq 40 51); do
  echo "|$n|" $(grep -h -E ":.{$((n*2))}$" block.* | grep -v h | cut -d: -f1 | sort -n) "|" |\
    sed -e "s% \([0-9][0-9]*\)% [\1](${url}\1)%g"
done | grep freecell
echo
echo "## Games solvable with safe auto play but unsolvable with max auto play"
echo
echo "| Moves | Games |"
echo "|-------|-------|"
for n in $(seq 30 51); do
  echo "|$n|" $(grep -h h block.* | grep -E ":.{$((n*2))}$" | cut -d: -f1 | sort -n) "|" |\
    sed -e "s% \([0-9][0-9]*\)% [\1](${url}\1)%g"
done | grep freecell
echo
echo "## Games unsolvable"
echo
echo "| Moves | Games |"
echo "|-------|-------|"
echo "|∞|" $(grep -h ":$" block.* | cut -d: -f1 | sort -n) "|" |\
    sed -e "s% \([0-9][0-9]*\)% [\1](${url}\1)%g"

