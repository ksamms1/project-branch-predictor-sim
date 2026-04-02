#ifndef SMITH_H
#define SMITH_H

// smith.h
// Simple Smith predictor (single global saturating counter).

#include "predictor_base.h"

class SmithPredictor : public PredictorBase
{
public:
    // counter_bits = number of bits in the saturating counter
    explicit SmithPredictor(int counter_bits);

    bool predict(uint64_t pc) override;
    void update(uint64_t pc, bool taken) override;
    void reset() override;
    void print_final_contents(std::ostream& os) const override;
    std::string name() const override;

private:
    int counter_bits_;
    int counter_;

    int max_value_;
    int threshold_;
};

#endif
