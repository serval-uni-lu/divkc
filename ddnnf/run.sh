#!/bin/bash

function run {
    splitter "$1" > "$1.log"
    cat "$1.log" "$1" > "$1.proj"
    proj "$1.proj"
    d4 -dDNNF "$1.proj.p" -out="$1.pnnf" | grep -E "^s "
    d4 -dDNNF "$1.proj.pup" -out="$1.unnf" | grep -E "^s "

    rm "$1.proj"
    # rm "$1.proj.p"
    # rm "$1.proj.pup"
    rm "$1.proj.pupp"
}
export -f run

run "$1"
