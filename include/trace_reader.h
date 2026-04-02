#ifndef TRACE_READER_H
#define TRACE_READER_H

// trace_reader.h
// Declares helper functions for reading branch trace files.

#include <string>
#include <vector>

#include "types.h"

// Read the entire trace file into memory as a list of BranchRecord values.
std::vector<BranchRecord> read_trace(const std::string& tracefile);

#endif
