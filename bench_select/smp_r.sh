#!/bin/bash

function run {
    # cp --parents "$HOME/Documents/Work/experiments/$1" "./f/"
    ./smp_rename "$1" > "$1.smp"
}
export -f run

find "$1" -name "*.cnf" | parallel -n 1 -P 4 run
