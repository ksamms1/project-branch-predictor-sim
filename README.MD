# project-branch-predictor-sim

Branch predictor simulator based on Machine Problem 2.

This project is organized so it can support:

1. Single-program simulation mode (MP2 baseline)
2. Multi-program mode CLI shape: ./sim multi <predictor_mode> <predictor_args...> <trace1> <trace2> [trace3 ...]
	Examples:
	./sim multi smith 3 traces/gcc_trace.txt traces/jpeg_trace.txt
	./sim multi bimodal 6 traces/gcc_trace.txt traces/jpeg_trace.txt traces/perl_trace.txt
	./sim multi gshare 9 3 traces/gcc_trace.txt traces/jpeg_trace.txt
	./sim multi hybrid 8 14 10 5 traces/gcc_trace.txt traces/jpeg_trace.txt	
---

## Current Status

- Single-program simulation mode is completed and validated
- Output matches professor-provided validation files exactly
- Ready for use by team members
- Multi-program simulation mode is not yet implemented in the main GitHub repo but will be in a branch when it is ready

---

## Repository Setup

Clone and build:

```bash
git clone git@github.com:ksamms1/project-branch-predictor-sim.git
cd project-branch-predictor-sim
make clean
make
```

### -------------   QUICK START   ------------ ###
    git clone git@github.com:ksamms1/project-branch-predictor-sim.git
    cd project-branch-predictor-sim
    make clean && make

    # Run baseline simulation
    ./sim gshare 9 3 traces/gcc_trace.txt

    # Validate correctness
    ./scripts/run_validation.sh

    # Generate experiment data
    ./scripts/run_experiments.sh
######  --------  END QUICK START  ------ #######



## Single-Program Usage

# Run from the project root directory.

# Smith Predictor
./sim smith 3 traces/gcc_trace.txt

# Bimodal Predictor
./sim bimodal 6 traces/gcc_trace.txt

# Gshare Predictor
./sim gshare 9 3 traces/gcc_trace.txt

# Hybrid Predictor
./sim hybrid 8 14 10 5 traces/gcc_trace.txt


## Validation
# Official Validation Suite (provided by Professor for the MP2 assignment)

# Runs all required validation tests and compares output with expected results.
chmod +x scripts/run_validation.sh
./scripts/run_validation.sh

# Formatted Validation (Official Validation Suite with nicer formatting - Recommended for debugging)

# Shows side-by-side output and pauses between tests (press Enter to continue).
chmod +x scripts/run_validation_w_formatting.sh
./scripts/run_validation_w_formatting.sh


## Experiment Scripts

# Generate CSV data for analysis (used in report / research phase):
chmod +x scripts/run_experiments.sh
./scripts/run_experiments.sh

# Outputs:
results/bimodal_results.csv
results/gshare_results.csv


## Project Structure
project-branch-predictor-sim/
├── include/                # Header files (predictors, interfaces, types)
├── src/
│   ├── main.cpp           # CLI entry point
│   ├── predictor_factory.cpp
│   ├── trace_reader.cpp
│   ├── predictors/        # Predictor implementations
│   └── simulation/
│       ├── single_program_sim.cpp   # Current execution mode
│       └── multiprogram_sim.cpp     # Currently in work
├── scripts/               # Validation and experiment scripts
├── traces/                # Input trace files
├── validation/            # Expected outputs for validation
├── results/               # Generated experiment data
├── report/                # Report materials (not required for execution)
├── Makefile
└── README.MD              # Directions to clone repo and use program



## How the Simulator Works Internally

At a high level, the simulator works like this:

1. `main.cpp` reads the command-line arguments and determines which predictor mode to run.
2. `predictor_factory.cpp` creates the correct predictor object (`Smith`, `Bimodal`, `Gshare`, or `Hybrid`).
3. `trace_reader.cpp` reads the branch trace file into memory as a sequence of branch records.
4. The single-program simulation loop processes each branch in order:
   - ask the predictor for a prediction
   - compare the prediction to the actual branch outcome
   - update prediction statistics
   - update the predictor state with the real outcome
5. After the trace finishes, the simulator prints:
   - command used
   - number of predictions
   - number of mispredictions
   - misprediction rate
   - final predictor contents

This structure keeps predictor logic separate from simulation flow, which makes it easier to preserve the original MP2 behavior while later adding a multi-program mode.
