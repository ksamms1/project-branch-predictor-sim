#include "bimodal.h"

// bimodal.cpp
// Implements the real bimodal predictor.
// The bimodal predictor uses PC bits to index a table of 3-bit
// saturating counters.

#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>

// Constructor
// () are used to initialize member variables in the order they are declared in the class definition
// This is more efficient than assignment in the constructor body and is required for const members or reference members.
BimodalPredictor::BimodalPredictor(int m2)
    : m2_(m2), // m2 determines the number of bits used for indexing, so table size = 2^m2
      table_(), // table is a vector of integers representing the saturating counters
      max_value_(7), // 3-bit counter max value is 7 (0 to 7)
      threshold_(4) // Threshold for predicting taken is 4 or higher
{
    if (m2_ <= 0)
    {
        throw std::runtime_error("Bimodal predictor m2 must be positive");
    }

    // MP2 specifies a table of 2^m2 entries.
    // Each entry is a 3-bit saturating counter initialized to 4.
    table_.assign(1 << m2_, 4);  // allocate a table of size 2^m2 and initialize all entries to 4. 1 << m2_ is equivalent to the m + 1 specified in the assignment.
}

// Return the table index for this PC.
// The assignment says to ignore the lowest 2 bits of the PC.
static std::size_t bimodal_index_from_pc(uint64_t pc, int m2)
{
    uint64_t shifted_pc = pc >> 2; // Shift right by 2 to basically delete the lowest 2 bits of the PC
    uint64_t mask = (1ULL << m2) - 1ULL; // Mask of 1111 to get the low bits if m = 4 for example

    // Apply mask to shifted_pc to get the low PC bits as the index into the branch prediction table of counters.
    return static_cast<std::size_t>(shifted_pc & mask);
}

// Predict taken if the selected counter is >= threshold.
// Otherwise predict not taken.
bool BimodalPredictor::predict(uint64_t pc)
{
    // Get the index into the table for this PC
    std::size_t index = bimodal_index_from_pc(pc, m2_);

    // Predict taken if the counter value is greater than or equal to the threshold
    // Otherwise predict not taken
    return (table_[index] >= threshold_);
}

// Update the selected counter using the actual branch outcome.
void BimodalPredictor::update(uint64_t pc, bool taken)
{
    // Get the index into the table for this PC
    std::size_t index = bimodal_index_from_pc(pc, m2_);

    if (taken)
    {
        // If the branch was taken, increment the counter if it's not already at the maximum value.
        if (table_[index] < max_value_)
        {
            table_[index]++;
        }
    }
    else
    {
        // If the branch was not taken, decrement the counter if it's not already at the minimum value (0).
        if (table_[index] > 0)
        {
            table_[index]--;
        }
    }
}

// Reset all counters back to the initial value 4.
void BimodalPredictor::reset()
{
    table_.assign(1 << m2_, 4);
}

// Print the final table contents in MP2 format.
void BimodalPredictor::print_final_contents(std::ostream& os) const
{
    os << "FINAL BIMODAL CONTENTS" << std::endl;

    // Print each index and its corresponding counter value.
    for (std::size_t i = 0; i < table_.size(); i++)
    {
        os << i << "\t" << table_[i] << std::endl;
    }
}

// Return predictor name.
std::string BimodalPredictor::name() const
{
    return "bimodal";
}
