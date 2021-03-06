#! /bin/bash

NUM_POINTS=10
RANGE=5000
STEP=RANGE/NUM_POINTS
mkdir -p ./test_data
OUTPUT_FILE=d4
#sed "${NUM}q;d" file -- to get a specific line from a file

rm ./test_data/$OUTPUT_FILE.data
rm ./test_data/$OUTPUT_FILE.labels
rm ./test_data/$OUTPUT_FILE.ipdata
rm ./test_data/$OUTPUT_FILE.iplabels

./server &

for ((i=STEP; i<RANGE; i+=STEP)); do
    echo Collecting data for $i:
    echo ip $i >> ./test_data/$OUTPUT_FILE.iplabels
    #echo net $i >> ./test_data/$OUTPUT_FILE.labels
    { time ./mcache_iptest $i 2>/dev/null ; } 2>> ./test_data/$OUTPUT_FILE.ipdata
    #{ time ./mcache_test $i 2>/dev/null ; } 2>> ./test_data/$OUTPUT_FILE.data
done

# pkill server &> /dev/null
echo All tests successful.
exit 0
