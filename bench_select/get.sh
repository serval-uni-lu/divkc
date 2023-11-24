#!/bin/bash

function run {
    cp --parents "$HOME/Documents/Work/experiments/$1" "./fd/"
}
export -f run

cat "$1" | cut -d ',' -f 1 | grep -v 'file' | parallel -n 1 -P 1 run
