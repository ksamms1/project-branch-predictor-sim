#!/bin/bash

# run_multi_experiments.sh
# Generate CSV data for multi-program branch predictor experiments.

set -e

mkdir -p results

echo "Cleaning project..."
make clean

echo "Building project..."
make

OUTPUT_FILE="results/multi_program_results.csv"

echo "predictor,slice,trace_set,total_predictions,total_mispredictions,total_misprediction_rate,program0_trace,program0_predictions,program0_mispredictions,program0_misprediction_rate,program1_trace,program1_predictions,program1_mispredictions,program1_misprediction_rate" > "$OUTPUT_FILE"

# Slice values to test
SLICES=(1 2 5 10 25 50 100)

# Trace pairs to test
TRACE_PAIRS=(
  "gcc jpeg"
  "gcc perl"
  "jpeg perl"
)

# Predictor configurations to test
# Format:
#   label|mode|args
PREDICTORS=(
  "smith_3|smith|3"
  "bimodal_6|bimodal|6"
  "gshare_9_3|gshare|9 3"
  "hybrid_8_14_10_5|hybrid|8 14 10 5"
)

for predictor_entry in "${PREDICTORS[@]}"
do
    IFS='|' read -r predictor_label predictor_mode predictor_args <<< "$predictor_entry"

    for slice in "${SLICES[@]}"
    do
        for pair in "${TRACE_PAIRS[@]}"
        do
            read -r trace_a trace_b <<< "$pair"

            cmd="./sim multi $slice $predictor_mode $predictor_args traces/${trace_a}_trace.txt traces/${trace_b}_trace.txt"
            echo "Running: $cmd"

            output=$($cmd)

            total_predictions=$(echo "$output" | grep "total predictions:" | awk '{print $3}')
            total_mispredictions=$(echo "$output" | grep "total mispredictions:" | awk '{print $3}')
            total_rate=$(echo "$output" | grep "total misprediction rate:" | awk '{print $4}' | tr -d '%')

            program0_trace=$(echo "$output" | awk '
                /program 0:/ {found=1; next}
                found && /tracefile:/ {print $2; exit}
            ')

            program0_predictions=$(echo "$output" | awk '
                /program 0:/ {found=1; next}
                found && /predictions:/ {print $2; exit}
            ')

            program0_mispredictions=$(echo "$output" | awk '
                /program 0:/ {found=1; next}
                found && /mispredictions:/ {print $2; exit}
            ')

            program0_rate=$(echo "$output" | awk '
                /program 0:/ {found=1; next}
                found && /misprediction rate:/ {print $3; exit}
            ' | tr -d '%')

            program1_trace=$(echo "$output" | awk '
                /program 1:/ {found=1; next}
                found && /tracefile:/ {print $2; exit}
            ')

            program1_predictions=$(echo "$output" | awk '
                /program 1:/ {found=1; next}
                found && /predictions:/ {print $2; exit}
            ')

            program1_mispredictions=$(echo "$output" | awk '
                /program 1:/ {found=1; next}
                found && /mispredictions:/ {print $2; exit}
            ')

            program1_rate=$(echo "$output" | awk '
                /program 1:/ {found=1; next}
                found && /misprediction rate:/ {print $3; exit}
            ' | tr -d '%')

            trace_set="${trace_a}+${trace_b}"

            echo "${predictor_label},${slice},${trace_set},${total_predictions},${total_mispredictions},${total_rate},${program0_trace},${program0_predictions},${program0_mispredictions},${program0_rate},${program1_trace},${program1_predictions},${program1_mispredictions},${program1_rate}" >> "$OUTPUT_FILE"
        done
    done
done

echo ""
echo "Multi-program experiment data written to $OUTPUT_FILE"