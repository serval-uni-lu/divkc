#!/bin/bash

function to_png {
  dot -Tpng "$1" -o "$1.png"
}
export -f to_png

function run {
  mc=$(d4 "$1" -out="$1.nnf" | grep -E -e "^s" | sed 's/^s //g')
  log=$(./build/dnnf "$1.nnf")
  dot -Tpng "$1.nnf.dot" -o "$1.nnf.dot.png" 2> /dev/null
  dot -Tpng "$1.nnf.old.dot" -o "$1.nnf.old.dot.png" 2> /dev/null
  echo "$log, $mc"
}
export -f run

mkdir -p build
cd build
if make -j8 ; then
    cd ..
    find benchmarks -name "*.cnf" | parallel -n 1 -P 7 run
fi
