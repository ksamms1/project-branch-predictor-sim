#!/bin/bash

# run_single_baselines.sh
# Generate CSV data for single-program baseline runs.

set -e

mkdir -p results

echo "Cleaning project..."
make clean

echo "Building project..."
make

OUTPUT_FILE="results/single_program_baselines.csv"

echo "run_id,predictor,predictor_args,tracefile,trace_name,predictions,mispredictions,misprediction_rate" > "$OUTPUT_FILE"

# Predictor configurations:
# label|mode|args
PREDICTORS=(
  "smith_3|smith|3"
  "bimodal_6|bimodal|6"
  "gshare_9_3|gshare|9 3"
  "hybrid_8_14_10_5|hybrid|8 14 10 5"
)

TRACES=(gcc jpeg perl)

for predictor_entry in "${PREDICTORS[@]}"
do
    IFS='|' read -r predictor_label predictor_mode predictor_args <<< "$predictor_entry"

    for trace_name in "${TRACES[@]}"
    do
        tracefile="traces/${trace_name}_trace.txt"
        run_id="single_${predictor_label}_${trace_name}"

        cmd="./sim $predictor_mode $predictor_args $tracefile"
        echo "Running: $cmd"

        output=$($cmd)

        predictions=$(echo "$output" | grep "number of predictions:" | awk '{print $4}')
        mispredictions=$(echo "$output" | grep "number of mispredictions:" | awk '{print $4}')
        misprediction_rate=$(echo "$output" | grep "misprediction rate:" | awk '{print $3}' | tr -d '%')

        echo "${run_id},${predictor_mode},\"${predictor_args}\",${tracefile},${trace_name},${predictions},${mispredictions},${misprediction_rate}" >> "$OUTPUT_FILE"
    done
done

echo ""
echo "Single-program baseline data written to $OUTPUT_FILE"