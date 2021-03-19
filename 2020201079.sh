#!/bin/bash

#!/bin/bash
#PROG_NAME=$1
PATHR=$1
PATHS=$2
mergeType=$3
M=$4

if [[ "$mergeType" == "hash" ]]; then
    echo "hash join is being performed"
    g++ -Werror "hashMergeJoin.cpp" -o a.out
    if [[ $? == 0 ]]; then
        ./a.out "$PATHR" "$PATHS" "$M"
    fi
else
    echo "sort join is being performed"
    g++ -Werror "sortMergeJoin.cpp" -o a.out
    if [[ $? == 0 ]]; then
        ./a.out "$PATHR" "$PATHS" "$M"
    fi
fi
