#!/bin/bash

splt=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc /divkc/wrap 16000 3600 /divkc/splitter --cnf "$1" 2>&1 1> "$1.log")
cat "$1.log" "$1" > "$1.proj"
prj=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc /divkc/wrap 16000 3600 /divkc/projection --cnf "$1.proj" 2>&1)

gp=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc /divkc/wrap 16000 3600 /divkc/d4 -dDNNF "$1.proj.p" -out="$1.pnnf" 2>&1)
gu=$(docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc /divkc/wrap 16000 3600 /divkc/d4 -dDNNF "$1.proj.pup" -out="$1.unnf" 2>&1)

rm -f "$1.proj"
rm -f "$1.proj.p"
rm -f "$1.proj.pup"
rm -f "$1.log"

nbv=$(cat "$1" | grep -E "^p cnf " | head -n 1 | cut -d ' ' -f 3)
nbc=$(cat "$1" | grep -E "^p cnf " | head -n 1 | cut -d ' ' -f 4)

gp_mc=$(echo "$gp" | grep -E "^s " | sed 's/^s //g')
gu_mc=$(echo "$gu" | grep -E "^s " | sed 's/^s //g')

echo "               file,low,high"
echo "cnf.bound.csv: $1, $gp_mc, $gu_mc"

echo ""
echo "             file,#v,#c"
echo "cnf.cls.csv: $1, $nbv, $nbc"

t=$(echo "$gp" | grep -E "^/divkc/d4" | sed 's/\/divkc\/d4 -dDNNF .* -out=//g;s/.pnnf//g')
echo ""
echo "          file, state, mem, time"
echo "pnnf.csv: $t"

t=$(echo "$gu" | grep -E "^/divkc/d4" | sed 's/\/divkc\/d4 -dDNNF .* -out=//g;s/.pnnf//g')
echo ""
echo "          file, state, mem, time"
echo "unnf.csv: $t"

t=$(echo "$splt" | grep -E "^/divkc/splitter --cnf " | sed 's/^\/divkc\/splitter --cnf //g')
echo ""
echo "           file, state, mem, time"
echo "split.csv: $t"

t=$(echo "$prj" | grep -E "^/divkc/projection --cnf " | sed 's/\/divkc\/projection --cnf //g')
echo ""
echo "          file, state, mem, time"
echo "proj.csv: $t"
