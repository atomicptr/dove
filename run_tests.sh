#!/usr/bin/env bash
rm -rf bin
mkdir bin

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

for file in ./tests/*.cpp; do
    executable="./bin/$(basename "${file%.*}").out"
    g++ -std=c++23 "$file" -o "$executable"

    ("$executable")
    res="$?"

    if [[ "$res" == 0 ]]; then
        echo -e "\t${GREEN}[SUCCESS]$NC\tOK\t\t$file"
    else
        echo -e "\t${RED}[FAIL]$NC\t\tTest failed\t$file"
        exit 1
    fi
done

exit 0
