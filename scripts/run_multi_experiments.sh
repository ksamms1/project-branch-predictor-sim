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

echo "run_id,scheduler,slice,predictor,predictor_args,program_count,trace_set,program_index,tracefile,trace_name,branches_loaded,branches_executed,predictions,mispredictions,misprediction_rate,total_predictions,total_mispredictions,total_misprediction_rate,scheduler_rounds" > "$OUTPUT_FILE"

# Slice values to test
SLICES=(1 2 5 10 25 50 100)

# Trace sets to test
TRACE_SETS=(
  "gcc jpeg"
  "gcc perl"
  "jpeg perl"
)

# Predictor configurations:
# label|mode|args
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
        for trace_set in "${TRACE_SETS[@]}"
        do
            read -r -a trace_array <<< "$trace_set"

            cmd="./sim multi $slice $predictor_mode $predictor_args"
            trace_set_name=""

            for trace_name in "${trace_array[@]}"
            do
                cmd="$cmd traces/${trace_name}_trace.txt"

                if [ -z "$trace_set_name" ]; then
                    trace_set_name="$trace_name"
                else
                    trace_set_name="${trace_set_name}+${trace_name}"
                fi
            done

            run_id="multi_${slice}_${predictor_label}_${trace_set_name}"

            echo "Running: $cmd"

            output=$($cmd)

            scheduler=$(echo "$output" | grep "^scheduler:" | awk '{print $2}')
            scheduler_rounds=$(echo "$output" | grep "^scheduler rounds:" | awk '{print $3}')
            total_predictions=$(echo "$output" | grep "^total predictions:" | awk '{print $3}')
            total_mispredictions=$(echo "$output" | grep "^total mispredictions:" | awk '{print $3}')
            total_misprediction_rate=$(echo "$output" | grep "^total misprediction rate:" | awk '{print $4}' | tr -d '%')

            program_count=${#trace_array[@]}

            for ((program_index=0; program_index<program_count; program_index++))
            do
                tracefile=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^tracefile:/ {print $2; exit}
                ')

                trace_name=$(echo "$tracefile" | sed 's#traces/##' | sed 's/_trace\.txt//')

                branches_loaded=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^branches loaded:/ {print $3; exit}
                ')

                branches_executed=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^branches executed:/ {print $3; exit}
                ')

                predictions=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^predictions:/ {print $2; exit}
                ')

                mispredictions=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^mispredictions:/ {print $2; exit}
                ')

                misprediction_rate=$(echo "$output" | awk -v idx="$program_index" '
                    $0 ~ "^program " idx ":" {found=1; next}
                    found && /^misprediction rate:/ {print $3; exit}
                ' | tr -d '%')

                echo "${run_id},${scheduler},${slice},${predictor_mode},\"${predictor_args}\",${program_count},${trace_set_name},${program_index},${tracefile},${trace_name},${branches_loaded},${branches_executed},${predictions},${mispredictions},${misprediction_rate},${total_predictions},${total_mispredictions},${total_misprediction_rate},${scheduler_rounds}" >> "$OUTPUT_FILE"
            done
        done
    done
done

echo ""
echo "Multi-program experiment data written to $OUTPUT_FILE"