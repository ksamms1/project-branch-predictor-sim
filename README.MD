# project-branch-predictor-sim

Branch predictor simulator based on Machine Problem 2.

This project is organized so it can support:

1. Single-program simulation mode (MP2 baseline)
2. Multi-program simulation mode (shared predictor with time slicing)

## Multi-Program CLI format:
```bash
./sim multi <slice> <predictor_mode> <predictor_args...> <trace1> <trace2> [trace3 ...]

Examples:
./sim multi 1 smith 3 traces/gcc_trace.txt traces/jpeg_trace.txt
./sim multi 5 bimodal 6 traces/gcc_trace.txt traces/jpeg_trace.txt traces/perl_trace.txt
./sim multi 10 gshare 9 3 traces/gcc_trace.txt traces/jpeg_trace.txt
./sim multi 25 hybrid 8 14 10 5 traces/gcc_trace.txt traces/jpeg_trace.txt
```
This mode uses a shared branch predictor across all input traces with round-robin scheduling.


## Current Status

- Single-program simulation mode is completed and validated
- Output matches professor-provided validation files exactly
- Ready for use by team members
- This branch (`feature/multi-program`) is the primary working version of the project
- It is intended to replace the original main branch after validation
- All multi-program experiments should be run from this branch
- Supports:
  - multiple trace files
  - shared predictor state
  - configurable time slicing
  - round-robin scheduling
  - per-program and total statistics
  - CSV export scripts for experiment data generation


---

## Repository Setup

Clone and build:

```bash
git clone git@github.com:ksamms1/project-branch-predictor-sim.git
cd project-branch-predictor-sim
git checkout feature/multi-program
make clean
make
```

### -------------   QUICK START   ------------ ###
   ```bash
   git clone git@github.com:ksamms1/project-branch-predictor-sim.git
   cd project-branch-predictor-sim
   git checkout feature/multi-program
   make clean && make

   # Run baseline simulation
   ./sim gshare 9 3 traces/gcc_trace.txt

   # Run multi-program simulation
   ./sim multi 5 gshare 9 3 traces/gcc_trace.txt traces/jpeg_trace.txt

   # Validate correctness
   ./scripts/run_validation.sh

   # Generate single-program baseline data
   ./scripts/run_single_baselines.sh

   # Generate multi-program experiment data
   ./scripts/run_multi_experiments.sh
   ```
######  --------  END QUICK START  ------ #######



## Single-Program Usage

Run from the project root directory.

```bash
# Smith
./sim smith 3 traces/gcc_trace.txt

# Bimodal
./sim bimodal 6 traces/gcc_trace.txt

# Gshare
./sim gshare 9 3 traces/gcc_trace.txt

# Hybrid
./sim hybrid 8 14 10 5 traces/gcc_trace.txt
```

## Validation
# Official Validation Suite (provided by Professor for the MP2 assignment)

# Runs all required validation tests and compares output with expected results.
```bash
chmod +x scripts/run_validation.sh
./scripts/run_validation.sh
```
# Formatted Validation (Official Validation Suite with nicer formatting - Recommended for debugging)

# Shows side-by-side output and pauses between tests (press Enter to continue).
```bash
chmod +x scripts/run_validation_w_formatting.sh
./scripts/run_validation_w_formatting.sh
```

## Experiment Scripts
These scripts are the primary interface for generating experiment data.
The simulator itself does not produce CSV files directly.

# Generate single-program baseline data
```bash
chmod +x scripts/run_single_baselines.sh
./scripts/run_single_baselines.sh
```
# Output:
```bash
results/single_program_baselines.csv
```
# Generate multi-program experiment data
```bash
chmod +x scripts/run_multi_experiments.sh
./scripts/run_multi_experiments.sh
```
# Output:
```bash
results/multi_program_results.csv
```

## Project Structure
project-branch-predictor-sim/
├── include/                # Header files (predictors, interfaces, types)
├── src/
│   ├── main.cpp           # CLI entry point
│   ├── predictor_factory.cpp
│   ├── trace_reader.cpp
│   ├── predictors/        # Predictor implementations
│   └── simulation/
│       ├── single_program_sim.cpp   # Single-program simulation mode
│       └── multiprogram_sim.cpp     # Multi-program simulation mode
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
6. In multi-program mode:
   - multiple traces are loaded
   - a single shared predictor is used
   - programs are scheduled in round-robin order
   - each program executes a fixed number of branches (time slice)
   - statistics are tracked per program and globally

This structure keeps predictor logic separate from simulation flow, which makes it easier to preserve the original MP2 behavior while later adding a multi-program mode.
