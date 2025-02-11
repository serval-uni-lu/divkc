#!/bin/bash

function run {
    splitter "$1" > "$1.log"
    r=$(cat "$1.log")

    if [ "$r" = "c p show 0" ] ; then
        echo "empty split: $1"
        rm "$1.log"
        exit 0
    fi

    cat "$1.log" "$1" > "$1.proj"
    r=$(proj "$1.proj")
    pmc=$(d4 -dDNNF "$1.proj.p" -out="$1.pnnf" | grep -E "^s " | sed 's/^s //g')
    umc=$(d4 -dDNNF "$1.proj.pup" -out="$1.unnf" | grep -E "^s " | sed 's/^s //g')

    rm "$1.proj"
    rm "$1.proj.p"
    rm "$1.proj.pup"
    # rm "$1.proj.pupp"

    # if [ "${#pmc}" -lt 5 ] ; then
    #     echo "skipping $1"
    #     rm "$1.pnnf"
    #     rm "$1.unnf"
    #     exit 0
    # fi

    op=$(julia -t 4 "src/main.jl" "$1")
    jpmc=$(echo "$op" | grep -E "^sp " | sed 's/^sp //g')
    jumc=$(echo "$op" | grep -E "^su " | sed 's/^su //g')
    jamc=$(echo "$op" | grep -E "^s " | sed 's/^s //g')

    if [ "$pmc" != "$jpmc" ] ; then
        echo "$1, pmc, $pmc, $jpmc"
    fi
    if [ "$umc" != "$jumc" ] ; then
        echo "$1, umc, $umc, $jumc"
    fi

    echo "$1, $pmc, $umc, $jamc"

    rm "$1.pnnf"
    rm "$1.unnf"
}
export -f run

run "$1"
