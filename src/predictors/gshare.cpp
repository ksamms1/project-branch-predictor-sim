#include "gshare.h"

// gshare.cpp
// Implements the real gshare predictor.
// Gshare uses global branch history together with PC bits to form the index.

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

// Compute gshare table index from PC and global history register.
static std::size_t gshare_index_from_pc(uint64_t pc, int m1, int n, uint32_t ghr)
{
    // Ignore the lowest 2 bits of the PC.
    uint32_t shifted_pc = static_cast<uint32_t>(pc >> 2);

    // Keep the lower m1 bits.
    uint32_t pc_low_m1 = shifted_pc & ((1U << m1) - 1U);

    // If n == 0, this reduces to bimodal indexing.
    if (n == 0)
    {
        return static_cast<std::size_t>(pc_low_m1);
    }

    // Use the lower n bits for XOR with the GHR.
    // Keep the upper (m1 - n) bits unchanged.
    uint32_t history_mask = (1U << n) - 1U;

    uint32_t lower_pc_bits = pc_low_m1 & history_mask;
    uint32_t upper_pc_bits = pc_low_m1 >> n;

    uint32_t xor_result = lower_pc_bits ^ ghr;

    uint32_t index = (upper_pc_bits << n) | xor_result;

    return static_cast<std::size_t>(index);
}

// Constructor
GsharePredictor::GsharePredictor(int m1, int n)
    : m1_(m1),
      n_(n),
      ghr_(0),
      table_(),
      max_value_(7),
      threshold_(4)
{
    if (m1_ <= 0)
    {
        throw std::runtime_error("Gshare predictor m1 must be positive");
    }

    if (n_ < 0)
    {
        throw std::runtime_error("Gshare predictor n cannot be negative");
    }

    if (n_ > m1_)
    {
        throw std::runtime_error("Gshare predictor n cannot be greater than m1");
    }

    // MP2 specifies a table of 2^m1 entries.
    // Each entry is a 3-bit saturating counter initialized to 4.
    table_.assign(1 << m1_, 4);
}

// Predict taken if selected counter >= threshold.
bool GsharePredictor::predict(uint64_t pc)
{
    return predict_with_current_history(pc);
}

// Same as predict(), but named clearly for hybrid use.
bool GsharePredictor::predict_with_current_history(uint64_t pc) const
{
    std::size_t index = gshare_index_from_pc(pc, m1_, n_, ghr_);
    return (table_[index] >= threshold_);
}

// Update only the selected table counter.
// This does NOT move the global history register.
void GsharePredictor::update_counter_only(uint64_t pc, bool taken)
{
    std::size_t index = gshare_index_from_pc(pc, m1_, n_, ghr_);

    if (taken)
    {
        if (table_[index] < max_value_)
        {
            table_[index]++;
        }
    }
    else
    {
        if (table_[index] > 0)
        {
            table_[index]--;
        }
    }
}

// Update only the global history register.
// This does NOT change any table counters.
void GsharePredictor::update_history_only(bool taken)
{
    if (n_ > 0)
    {
        // Shift right by 1.
        ghr_ = ghr_ >> 1;

        // Insert newest outcome into the most significant history bit.
        if (taken)
        {
            ghr_ = ghr_ | (1U << (n_ - 1));
        }

        // Keep only n bits.
        ghr_ = ghr_ & ((1U << n_) - 1U);
    }
}

// Full normal gshare update: update counter, then update history.
void GsharePredictor::update(uint64_t pc, bool taken)
{
    update_counter_only(pc, taken);
    update_history_only(taken);
}

// Reset table and global history register.
void GsharePredictor::reset()
{
    ghr_ = 0;
    table_.assign(1 << m1_, 4);
}

// Print final table contents in MP2 format.
void GsharePredictor::print_final_contents(std::ostream& os) const
{
    os << "FINAL GSHARE CONTENTS" << std::endl;

    for (std::size_t i = 0; i < table_.size(); i++)
    {
        os << i << "\t" << table_[i] << std::endl;
    }
}

// Return predictor name.
std::string GsharePredictor::name() const
{
    return "gshare";
}
