// multiprogram_sim.cpp
// Multi-program shared-predictor simulation with configurable time slice.

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "multiprogram_sim.h"
#include "predictor_base.h"
#include "predictor_factory.h"
#include "trace_reader.h"
#include "types.h"

// Holds one program's trace and execution progress.
struct ProgramTrace
{
    std::string tracefile;
    std::vector<BranchRecord> branches;
    std::size_t next_index = 0;
    SimulationStats stats;
};

// Execute ONE branch for a program.
static bool execute_one_branch(
    ProgramTrace& program,
    PredictorBase& predictor,
    SimulationStats& total_stats)
{
    if (program.next_index >= program.branches.size())
    {
        return false;
    }

    const BranchRecord& branch = program.branches[program.next_index];

    bool predicted_taken = predictor.predict(branch.pc);

    program.stats.predictions++;
    total_stats.predictions++;

    if (predicted_taken != branch.taken)
    {
        program.stats.mispredictions++;
        total_stats.mispredictions++;
    }

    predictor.update(branch.pc, branch.taken);

    program.next_index++;
    return true;
}

int run_multi_program_sim(
    std::size_t time_slice,
    const std::string& predictor_mode,
    const std::vector<std::string>& predictor_args,
    const std::vector<std::string>& tracefiles
)
{
    std::unique_ptr<PredictorBase> predictor =
        create_predictor(predictor_mode, predictor_args);

    std::vector<ProgramTrace> programs;

    for (const auto& tf : tracefiles)
    {
        ProgramTrace program;
        program.tracefile = tf;
        program.branches = read_trace(tf);
        programs.push_back(program);
    }

    SimulationStats total_stats;

    bool work_remaining = true;

    while (work_remaining)
    {
        work_remaining = false;

        for (auto& program : programs)
        {
            if (program.next_index >= program.branches.size())
            {
                continue;
            }

            work_remaining = true;

            for (std::size_t i = 0; i < time_slice; i++)
            {
                if (!execute_one_branch(program, *predictor, total_stats))
                {
                    break;
                }
            }
        }
    }

    std::cout << "MULTI-PROGRAM MODE" << std::endl;
    std::cout << "scheduler:\tround-robin" << std::endl;
    std::cout << "time slice:\t" << time_slice << std::endl;
    std::cout << "predictor:\t" << predictor_mode << std::endl;

    std::cout << "predictor args:";
    for (const auto& arg : predictor_args)
    {
        std::cout << " " << arg;
    }
    std::cout << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "total predictions:\t" << total_stats.predictions << std::endl;
    std::cout << "total mispredictions:\t" << total_stats.mispredictions << std::endl;

    double total_rate = 0.0;
    if (total_stats.predictions > 0)
    {
        total_rate = 100.0 *
            static_cast<double>(total_stats.mispredictions) /
            static_cast<double>(total_stats.predictions);
    }

    std::cout << "total misprediction rate:\t" << total_rate << "%" << std::endl;

    std::cout << "per-program results:" << std::endl;

    for (std::size_t i = 0; i < programs.size(); i++)
    {
        double rate = 0.0;
        if (programs[i].stats.predictions > 0)
        {
            rate = 100.0 *
                static_cast<double>(programs[i].stats.mispredictions) /
                static_cast<double>(programs[i].stats.predictions);
        }

        std::cout << "  [" << i << "] " << programs[i].tracefile << std::endl;
        std::cout << "    predictions:\t" << programs[i].stats.predictions << std::endl;
        std::cout << "    mispredictions:\t" << programs[i].stats.mispredictions << std::endl;
        std::cout << "    misprediction rate:\t" << rate << "%" << std::endl;
    }

    return 0;
}