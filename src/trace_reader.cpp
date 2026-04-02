#include "trace_reader.h"

// trace_reader.cpp
// Reads a branch trace file and converts each line into a BranchRecord.
// BranchRecord defined in types.h which is imported by trace_reader.h

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Read the entire trace file into memory.
// const = read-only, reference = no copy, reference original string
std::vector<BranchRecord> read_trace(const std::string& tracefile) // tracefile = path to the trace file to read
{
    std::ifstream input(tracefile); // input file stream to read from the trace file

    if (!input.is_open())
    {
        throw std::runtime_error("failed to open trace file: " + tracefile);
    }

    std::vector<BranchRecord> records; // branch records read from input tracefile, to be returned to caller
    std::string line; // variable to hold each line read from the trace file

    while (std::getline(input, line))  // getline(read one line from input file stream into line variable, return true if successful, false if end of file or error)
    {
        // Skip empty lines if any appear
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line); // input string stream to parse the line into components (PC and outcome)

        std::string pc_text; // variable to hold the PC text read from the line (e.g. "0x4004f4")
        char outcome_char = 'n'; // variable to hold the outcome character read from the line ('t' for taken, 'n' for not taken)

        // parse the line into whitespace separated pc_text and outcome_char variables, return false if parsing fails
        if (!(iss >> pc_text >> outcome_char))
        {
            throw std::runtime_error("invalid trace line: " + line);
        }

        BranchRecord record; // create a BranchRecord struct to hold the parsed PC and outcome for this line

        // Convert hex text such as pc_text = "0x4004f4" into record.pc = integer
        // stoull (string to unsigned long long): convert pc_text from hex string to unsigned long long integer and store in record.pc
        // nullptr => parsing stops at the first invalid character (we get whatever was valid before the invalid character)
        // 16 means the input is in base 16 (hexadecimal)
        record.pc = std::stoull(pc_text, nullptr, 16); // record.pc = the integer value of the PC address for this branch instruction

        // Convert outcome character into bool
        if (outcome_char == 't')
        {
            record.taken = true; // set record.taken to true if outcome_char is 't'
        }
        else if (outcome_char == 'n')
        {
            record.taken = false; // set record.taken to false if outcome_char is 'n'
        }
        else
        {
            throw std::runtime_error("invalid branch outcome in line: " + line);
        }

        records.push_back(record); // add the constructed branch record to the Branch records vector
    }


    return records;
}
