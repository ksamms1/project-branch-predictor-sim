// multiprogram_sim.cpp
// Early implementation for future multi-program shared-predictor simulation mode.
// This step creates the shared predictor, loads all trace files, and reports
// what was loaded. Interleaved execution will be added later.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "multiprogram_sim.h"
#include "predictor_base.h"
#include "predictor_factory.h"
#include "trace_reader.h"
#include "types.h"

// Simple container for one program's trace data.
struct ProgramTrace
{
    std::string tracefile;
    std::vector<BranchRecord> branches;
};

// Run the future multi-program simulation mode.
int run_multi_program_sim(
    const std::string& predictor_mode,
    const std::vector<std::string>& predictor_args,
    const std::vector<std::string>& tracefiles
)

{
    // Create one shared predictor instance.
    std::unique_ptr<PredictorBase> predictor =
        create_predictor(predictor_mode, predictor_args);

    // Store all loaded traces here.
    std::vector<ProgramTrace> programs;

    // Load each trace file into memory.
    for (std::size_t i = 0; i < tracefiles.size(); i++)
    {
        ProgramTrace program;
        program.tracefile = tracefiles[i];
        program.branches = read_trace(tracefiles[i]);
        programs.push_back(program);
    }

    // Report what was loaded.
    std::cout << "MULTI-PROGRAM MODE" << std::endl;
    std::cout << "Predictor mode: " << predictor_mode << std::endl;

    std::cout << "Predictor arguments:";
    for (std::size_t i = 0; i < predictor_args.size(); i++)
    {
        std::cout << " " << predictor_args[i];
    }
    std::cout << std::endl;

    // List loaded trace files and how many branches each contains.
    std::cout << "Loaded trace files:" << std::endl;
    for (std::size_t i = 0; i < programs.size(); i++)
    {
        std::cout << "  " << programs[i].tracefile
                  << " (" << programs[i].branches.size()
                  << " branches)" << std::endl;
    }

    // Placeholder for future interleaved execution logic.
    std::cout << "Shared predictor created successfully." << std::endl;
    std::cout << "Interleaved multi-program execution is not implemented yet."
              << std::endl;

    return 0;
}