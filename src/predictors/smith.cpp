#include "smith.h"

// smith.cpp
// Implements the simple Smith predictor.
// This predictor uses one global saturating counter for all branches.

#include <ostream>
#include <stdexcept>
#include <string>

// Constructor
SmithPredictor::SmithPredictor(int counter_bits)
    : counter_bits_(counter_bits),
      counter_(0),
      max_value_(0),
      threshold_(0)
{
    // The assignment expects the number of counter bits to be positive.
    if (counter_bits_ <= 0)
    {
        throw std::runtime_error("Smith predictor counter_bits must be positive");
    }

    // For a k-bit counter:
    // max_value = 2^k - 1
    // threshold = 2^(k-1)
    max_value_ = (1 << counter_bits_) - 1;
    threshold_ = (1 << (counter_bits_ - 1));

    // Initialize the counter to the middle value.
    // This matches the standard Smith predictor initialization.
    counter_ = threshold_;
}

// Return the current prediction.
// If counter >= threshold, predict taken.
// Otherwise, predict not taken.
bool SmithPredictor::predict(uint64_t pc)
{
    // pc is unused in Smith because there is only one global counter.
    (void)pc;

    return (counter_ >= threshold_);
}

// Update the single saturating counter using the actual outcome.
void SmithPredictor::update(uint64_t pc, bool taken)
{
    // pc is unused in Smith because there is only one global counter.
    (void)pc;

    if (taken)
    {
        if (counter_ < max_value_)
        {
            counter_++;
        }
    }
    else
    {
        if (counter_ > 0)
        {
            counter_--;
        }
    }
}

// Reset the predictor back to its initial state.
void SmithPredictor::reset()
{
    counter_ = threshold_;
}

// Print final contents in the format expected by the validation output.
void SmithPredictor::print_final_contents(std::ostream& os) const
{
    os << "FINAL COUNTER CONTENT:\t\t" << counter_ << std::endl;
}

// Return predictor name.
std::string SmithPredictor::name() const
{
    return "smith";
}
