#!/bin/bash

function run {
    nbv=$(cat "$1" | grep -E "^p cnf " | head -n 1 | cut -d ' ' -f 3)
    nbc=$(cat "$1" | grep -E "^p cnf " | head -n 1 | cut -d ' ' -f 4)

    echo "$1, $nbv, $nbc" > "$1.csv.cls"

    splt=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
        /divkc/wrap "64000" "1800" \
        /divkc/splitter --cnf "$1" 2>&1 1> "$1.log")

    echo "$splt" | grep -E "^/divkc/splitter --cnf " | sed 's/^\/divkc\/splitter --cnf //g' > "$1.csv.splt"

    r=$(cat "$1.log")
    if [ "$r" = "c p show 0" ] ; then
        echo "empty split: $1"
        rm "$1.log"
        exit 0
    fi

    if [ -z "$r" ] ; then
        echo "split failed: $1"
        rm "$1.log"
        exit 0
    fi

    cat "$1.log" "$1" > "$1.proj"

    prj=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
        /divkc/wrap "64000" "10800" \
        /divkc/projection --cnf "$1.proj" 2>&1)

    echo "$prj" | grep -E "^/divkc/projection --cnf " | sed 's/^\/divkc\/projection --cnf //g' > "$1.csv.prj"

    gp=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
        /divkc/wrap "16000" "3600" \
        /divkc/d4 -dDNNF "$1.proj.p" -out="$1.pnnf" 2>&1)

    echo "$gp" | grep -E "^/divkc/d4" | sed 's/^\/divkc\/d4 .* -out=//g;s/.pnnf//g' > "$1.csv.gp"

    gu=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
        /divkc/wrap "16000" "3600" \
        /divkc/d4 -dDNNF "$1.proj.pup" -out="$1.unnf" 2>&1)

    echo "$gu" | grep -E "^/divkc/d4" | sed 's/^\/divkc\/d4 .* -out=//g;s/.unnf//g' > "$1.csv.gu"

    pmc=$(echo "$gp" | grep -E "^s " | sed 's/^s //g')
    if [ -z "$pmc" ] ; then
          echo "failed to compile G_P: $1"
    fi

    umc=$(echo "$gu" | grep -E "^s " | sed 's/^s //g')
    if [ -z "$umc" ] ; then
          echo "failed to compile G_U: $1"
    fi

    rm -f "$1.proj"
    rm -f "$1.proj.p"
    rm -f "$1.proj.pup"
    rm -f "$1.log"

    if [ -z "$pmc" ] || [ -z "$umc" ] ; then
        echo "failed tom compile G_P or G_U, exiting now: $1"
        exit 0
    fi

    echo "$1, $pmc, $umc" > "$1.csv.bounds"

    for i in $(seq 1 4) ; do
        amc=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
            /divkc/wrap 64000 5400 \
            /divkc/appmc --alpha 0.01 --lnb 20 --epsilon 1.1 --nb 10000 --cnf "$1" 2>&1)
        echo "$amc" | grep -E "^/divkc/appmc" | sed 's/^.* --cnf //g' >> "$1.csv.r"
        tr=$(echo "$amc" | grep -E -v "^/divkc/appmc" | tail -n 2 | head -n 1)
        echo "$1, $tr" > "$1.csv.clt"
    done

    amcr=$(awk -F, '
        {               
            f1 = $1
            f2 = $2
            if($3 > max || NR == 1)
                max = $3
            sum += $4
        }
        END {                 
            print f2 "," max "," sum
        }' "$1.csv.r")
    rm "$1.csv.r"
    tr=$(cat "$1.csv.clt")
    echo "$tr,$amcr" > "$1.csv.clt.run"
}
export -f run

# $1 the folder to search
# $2 the header for the csv file
# $3 the name of the csv file
# $4 the file extension to search
function collect {
    echo "$2" > "$1.$3"
    find "$1" -name "*$4" -exec cat "{}" \; >> "$1.$3"
    find "$1" -name "*$4" -delete
}
export -f collect

find "$1" -name "*.cnf" | parallel -n 1 -P 1 run

collect "$1" "file, #v, #c" "cls.csv" "csv.cls"
collect "$1" "file, state, mem, time" "split.csv" "csv.splt"
collect "$1" "file, state, mem, time" "proj.csv" "csv.prj"
collect "$1" "file, state, mem, time" "pnnf.csv" "csv.gp"
collect "$1" "file, state, mem, time" "unnf.csv" "csv.gu"
collect "$1" "file,low,high" "bounds.csv" "csv.bounds"
collect "$1" "file,N,Y,Yl,Yh" "clt.csv" "csv.clt"
collect "$1" "file, N, Y, Yl, Yh, state, mem, time" "clt.run.csv" "csv.clt.run"
