#ifndef TYPES_H
#define TYPES_H

// types.h
// Shared basic data types used across the simulator.
// Keep this file simple so all other files can include it.

#include <cstdint>

// Represents one branch from the trace file.
// pc    = program counter address of the branch instruction
// taken = true if actual outcome was taken, false if not taken
struct BranchRecord
{
    uint64_t pc = 0;
    bool taken = false;
};

// Tracks overall simulation results.
// predictions    = total number of branches processed
// mispredictions = total number of wrong predictions
struct SimulationStats
{
    uint64_t predictions = 0;
    uint64_t mispredictions = 0;
};

#endif
