#ifndef GSHARE_H
#define GSHARE_H

// gshare.h
// Gshare branch predictor: table indexed by PC bits combined with
// global branch history.

#include <cstdint>
#include <vector>
#include "predictor_base.h"

class GsharePredictor : public PredictorBase
{
public:
    // m1 determines table size = 2^m1
    // n  determines number of history bits in the global history register
    GsharePredictor(int m1, int n);

    bool predict(uint64_t pc) override;
    void update(uint64_t pc, bool taken) override;
    void reset() override;
    void print_final_contents(std::ostream& os) const override;
    std::string name() const override;

    // Helper methods used by the hybrid predictor.
    // These let hybrid update gshare correctly:
    // - sometimes update only the gshare counter
    // - always update the gshare history register
    bool predict_with_current_history(uint64_t pc) const;
    void update_counter_only(uint64_t pc, bool taken);
    void update_history_only(bool taken);

private:
    int m1_;
    int n_;
    uint32_t ghr_;
    std::vector<int> table_;

    int max_value_;
    int threshold_;
};

#endif
