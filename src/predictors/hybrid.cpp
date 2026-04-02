#include "hybrid.h"

// hybrid.cpp
// Implements the real hybrid predictor.
// Hybrid chooses between bimodal and gshare using a chooser table.

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

// Compute chooser table index from PC.
// Hybrid chooser indexing ignores the lowest 2 bits of the PC
// and then uses the lower k bits.
static std::size_t chooser_index_from_pc(uint64_t pc, int k)
{
    uint64_t shifted_pc = pc >> 2;
    uint64_t mask = (1ULL << k) - 1ULL;
    return static_cast<std::size_t>(shifted_pc & mask);
}

// Constructor
HybridPredictor::HybridPredictor(int k, int m1, int n, int m2)
    : k_(k),
      chooser_(),
      bimodal_(m2),
      gshare_(m1, n),
      last_used_gshare_(false)
{
    if (k_ <= 0)
    {
        throw std::runtime_error("Hybrid predictor k must be positive");
    }

    // MP2 uses 2-bit chooser counters initialized to 1.
    chooser_.assign(1 << k_, 1);
}

// Predict using the chooser table.
// If chooser counter >= 2, choose gshare.
// Otherwise choose bimodal.
bool HybridPredictor::predict(uint64_t pc)
{
    std::size_t chooser_index = chooser_index_from_pc(pc, k_);

    bool bimodal_prediction = bimodal_.predict(pc);
    bool gshare_prediction = gshare_.predict_with_current_history(pc);

    if (chooser_[chooser_index] >= 2)
    {
        last_used_gshare_ = true;
        return gshare_prediction;
    }
    else
    {
        last_used_gshare_ = false;
        return bimodal_prediction;
    }
}

// Update chooser and selected predictor.
// Always update gshare history, even if bimodal was selected.
void HybridPredictor::update(uint64_t pc, bool taken)
{
    std::size_t chooser_index = chooser_index_from_pc(pc, k_);

    bool bimodal_prediction = bimodal_.predict(pc);
    bool gshare_prediction = gshare_.predict_with_current_history(pc);

    bool bimodal_correct = (bimodal_prediction == taken);
    bool gshare_correct = (gshare_prediction == taken);

    // Update chooser only if bimodal and gshare disagreed.
    if (bimodal_prediction != gshare_prediction)
    {
        if (gshare_correct && !bimodal_correct)
        {
            if (chooser_[chooser_index] < 3)
            {
                chooser_[chooser_index]++;
            }
        }
        else if (bimodal_correct && !gshare_correct)
        {
            if (chooser_[chooser_index] > 0)
            {
                chooser_[chooser_index]--;
            }
        }
    }

    // Update only the selected predictor's table.
    if (last_used_gshare_)
    {
        gshare_.update_counter_only(pc, taken);
    }
    else
    {
        bimodal_.update(pc, taken);
    }

    // Always update the gshare history register.
    gshare_.update_history_only(taken);
}

// Reset chooser and child predictors.
void HybridPredictor::reset()
{
    chooser_.assign(1 << k_, 1);
    bimodal_.reset();
    gshare_.reset();
    last_used_gshare_ = false;
}

// Print final chooser contents, then underlying predictor contents.
void HybridPredictor::print_final_contents(std::ostream& os) const
{
    os << "FINAL CHOOSER CONTENTS" << std::endl;

    for (std::size_t i = 0; i < chooser_.size(); i++)
    {
        os << i << "\t" << chooser_[i] << std::endl;
    }

    gshare_.print_final_contents(os);
    bimodal_.print_final_contents(os);
}

// Return predictor name.
std::string HybridPredictor::name() const
{
    return "hybrid";
}
