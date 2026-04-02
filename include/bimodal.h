#ifndef BIMODAL_H
#define BIMODAL_H

// bimodal.h
// Bimodal branch predictor: table indexed by PC bits, each entry is a saturating counter.

#include <vector>
#include "predictor_base.h"

class BimodalPredictor : public PredictorBase
{
public:
    // m2 determines table size = 2^m2
    explicit BimodalPredictor(int m2);

    bool predict(uint64_t pc) override;
    void update(uint64_t pc, bool taken) override;
    void reset() override;
    void print_final_contents(std::ostream& os) const override;
    std::string name() const override;

private:
    int m2_;
    std::vector<int> table_;

    int max_value_;
    int threshold_;
};

#endif
