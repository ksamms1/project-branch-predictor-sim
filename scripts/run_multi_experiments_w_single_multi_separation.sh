#!/bin/bash

# scripts/run_single_vs_multi_comparison.sh
# Generates one CSV that clearly separates:
#   - single-program baseline runs
#   - multi-program shared-predictor runs
#
# Needs to be adjusted to the final experiment plan regarding predictor configurations, 
#     slice values, trace pairs, totals vs per-program metrics, degredation calculations, etc.

set -e

mkdir -p results

echo "Cleaning project..."
make clean

echo "Building project..."
make

OUTPUT_FILE="results/single_vs_multi_comparison.csv"

echo "mode,predictor,slice,trace_set,program_index,tracefile,predictions,mispredictions,misprediction_rate" > "$OUTPUT_FILE"

# -----------------------------
# Configurations
# -----------------------------

SLICES=(1 2 5 10 25 50 100)

TRACE_PAIRS=(
  "gcc jpeg"
  "gcc perl"
  "jpeg perl"
)

PREDICTORS=(
  "smith_3|smith|3"
  "bimodal_6|bimodal|6"
  "gshare_9_3|gshare|9 3"
  "hybrid_8_14_10_5|hybrid|8 14 10 5"
)

# -----------------------------
# Single-program baselines
# -----------------------------

for predictor_entry in "${PREDICTORS[@]}"
do
    IFS='|' read -r predictor_label predictor_mode predictor_args <<< "$predictor_entry"

    for trace_name in gcc jpeg perl
    do
        cmd="./sim $predictor_mode $predictor_args traces/${trace_name}_trace.txt"
        echo "Running single: $cmd"

        output=$($cmd)

        predictions=$(echo "$output" | grep "number of predictions:" | awk '{print $4}')
        mispredictions=$(echo "$output" | grep "number of mispredictions:" | awk '{print $4}')
        rate=$(echo "$output" | grep "misprediction rate:" | awk '{print $3}' | tr -d '%')

        echo "single,${predictor_label},NA,${trace_name},0,${trace_name}_trace.txt,${predictions},${mispredictions},${rate}" >> "$OUTPUT_FILE"
    done
done

# -----------------------------
# Multi-program runs
# -----------------------------

for predictor_entry in "${PREDICTORS[@]}"
do
    IFS='|' read -r predictor_label predictor_mode predictor_args <<< "$predictor_entry"

    for slice in "${SLICES[@]}"
    do
        for pair in "${TRACE_PAIRS[@]}"
        do
            read -r trace_a trace_b <<< "$pair"

            cmd="./sim multi $slice $predictor_mode $predictor_args traces/${trace_a}_trace.txt traces/${trace_b}_trace.txt"
            echo "Running multi: $cmd"

            output=$($cmd)

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

            echo "multi,${predictor_label},${slice},${trace_set},0,${program0_trace},${program0_predictions},${program0_mispredictions},${program0_rate}" >> "$OUTPUT_FILE"
            echo "multi,${predictor_label},${slice},${trace_set},1,${program1_trace},${program1_predictions},${program1_mispredictions},${program1_rate}" >> "$OUTPUT_FILE"
        done
    done
done

echo ""
echo "Comparison data written to $OUTPUT_FILE"