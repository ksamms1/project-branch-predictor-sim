#ifndef HYBRID_H
#define HYBRID_H

// hybrid.h
// Hybrid branch predictor: uses a chooser table to decide whether
// to trust the bimodal predictor or the gshare predictor.

#include <vector>
#include "predictor_base.h"
#include "bimodal.h"
#include "gshare.h"

class HybridPredictor : public PredictorBase
{
public:
    // k  determines chooser table size = 2^k
    // m1 and n configure the gshare predictor
    // m2 configures the bimodal predictor
    HybridPredictor(int k, int m1, int n, int m2);

    bool predict(uint64_t pc) override;
    void update(uint64_t pc, bool taken) override;
    void reset() override;
    void print_final_contents(std::ostream& os) const override;
    std::string name() const override;

private:
    int k_;
    std::vector<int> chooser_;

    BimodalPredictor bimodal_;
    GsharePredictor gshare_;

    bool last_used_gshare_;
};

#endif
