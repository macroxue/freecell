if [[ $1 = 'v' ]]; then
  visualize=1
  shift
fi
deal=$1
beam=${2:-3}
while [[ $beam -le 16 ]]; do
  solution=$(./solver $deal $beam | grep "^$deal:" | tail -1)
  if [[ -n $solution ]]; then
    echo $beam:$solution
    if [[ $visualize -eq 1 ]]; then
      echo $solution | ./visualizer $deal
    fi
    exit
  fi
  beam=$((beam+1))
done
echo "No solution"
