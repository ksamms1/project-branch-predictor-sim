#!/bin/bash

# run_experiments.sh
# Generate experiment data for MP2 report.
# This script writes simple CSV files for bimodal and gshare results.

set -e

mkdir -p results

echo "Cleaning project..."
make clean

echo "Building project..."
make

# ------------------------------------------------------------
# Bimodal experiments
# For each benchmark, run m = 7 through 12
# ------------------------------------------------------------

echo "Generating bimodal results..."

echo "benchmark,m2,mispredictions,misprediction_rate" > results/bimodal_results.csv

for trace in gcc jpeg perl
do
    for m2 in 7 8 9 10 11 12
    do
        output=$(./sim bimodal $m2 traces/${trace}_trace.txt)

        mispredictions=$(echo "$output" | grep "number of mispredictions:" | awk '{print $4}')
        rate=$(echo "$output" | grep "misprediction rate:" | awk '{print $3}' | tr -d '%')

        echo "${trace},${m2},${mispredictions},${rate}" >> results/bimodal_results.csv
    done
done

# ------------------------------------------------------------
# Gshare experiments
# For each benchmark, run:
#   m = 7 through 12
#   n = 2, 4, 6, ... up to m
# ------------------------------------------------------------

echo "Generating gshare results..."

echo "benchmark,m1,n,mispredictions,misprediction_rate" > results/gshare_results.csv

for trace in gcc jpeg perl
do
    for m1 in 7 8 9 10 11 12
    do
        n=2
        while [ $n -le $m1 ]
        do
            output=$(./sim gshare $m1 $n traces/${trace}_trace.txt)

            mispredictions=$(echo "$output" | grep "number of mispredictions:" | awk '{print $4}')
            rate=$(echo "$output" | grep "misprediction rate:" | awk '{print $3}' | tr -d '%')

            echo "${trace},${m1},${n},${mispredictions},${rate}" >> results/gshare_results.csv

            n=$((n + 2))
        done
    done
done

echo "Experiment data generated in results/"
