#ifndef PREDICTOR_BASE_H
#define PREDICTOR_BASE_H

// predictor_base.h
// Base class for all branch predictors.
// Every predictor should support the same core operations so the simulator
// can use them through one common interface.

#include <cstdint>
#include <iosfwd>
#include <string>

class PredictorBase
{
public:
    // Virtual destructor is required for proper cleanup through base pointers
    virtual ~PredictorBase() = default;

    // Return the prediction for a branch at the given PC
    // true  = predict taken
    // false = predict not taken
    virtual bool predict(uint64_t pc) = 0;

    // Update the predictor state using the actual branch outcome
    // true  = actually taken
    // false = actually not taken
    virtual void update(uint64_t pc, bool taken) = 0;

    // Optional reset hook if we want to reuse predictor objects later
    virtual void reset() {}

    // Print the final table or counter contents in assignment format
    virtual void print_final_contents(std::ostream& os) const = 0;

    // Return a simple string name for the predictor
    virtual std::string name() const = 0;
};

#endif
