#!/bin/bash

function run {
    splitter "$1" > "$1.log"
    cat "$1.log" "$1" > "$1.proj"
    proj "$1.proj"
    pmc=$(d4 -dDNNF "$1.proj.p" -out="$1.pnnf" | grep -E "^s " | sed 's/^s //g')
    umc=$(d4 -dDNNF "$1.proj.pup" -out="$1.unnf" | grep -E "^s " | sed 's/^s //g')

    rm "$1.proj"
    rm "$1.proj.p"
    rm "$1.proj.pup"
    rm "$1.proj.pupp"

    if [ "${#pmc}" -lt 5 ] ; then
        echo "skipping $1"
        rm "$1.pnnf"
        rm "$1.unnf"
        exit 0
    fi

    op=$(julia "/home/oz/Documents/projects/papers/appNNF/ddnnf/src/main.jl" "$1")
    jpmc=$(echo "$op" | grep -E "^sp " | sed 's/^sp //g')
    jumc=$(echo "$op" | grep -E "^su " | sed 's/^su //g')

    if [ "$pmc" != "$jpmc" ] ; then
        echo "$1, pmc, $pmc, $jpmc"
    fi
    if [ "$umc" != "$jumc" ] ; then
        echo "$1, umc, $umc, $jumc"
    fi

    rm "$1.pnnf"
    rm "$1.unnf"
}
export -f run

run "$1"
