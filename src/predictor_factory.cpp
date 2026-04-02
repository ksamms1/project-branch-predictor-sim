#include "predictor_factory.h"

// predictor_factory.cpp
// Creates the correct predictor object based on the mode string
// and numeric command-line arguments.

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "smith.h"
#include "bimodal.h"
#include "gshare.h"
#include "hybrid.h"

// Create and return the correct predictor object.
std::unique_ptr<PredictorBase> create_predictor(
    const std::string& mode,
    const std::vector<std::string>& args)
{
    // Smith: sim smith <counter_bits> <tracefile>
    if (mode == "smith")
    {
        if (args.size() != 1)
        {
            throw std::runtime_error("smith mode requires 1 numeric argument");
        }

        int counter_bits = std::stoi(args[0]);

        // Create and return a SmithPredictor object from smith.h configured with the provided argument.
        return std::make_unique<SmithPredictor>(counter_bits);
    }

    // Bimodal: sim bimodal <M2> <tracefile>
    if (mode == "bimodal")
    {
        if (args.size() != 1)
        {
            throw std::runtime_error("bimodal mode requires 1 numeric argument");
        }

        int m2 = std::stoi(args[0]);

        // Create and return a BimodalPredictor object from bimodal.h configured with the provided argument.
        return std::make_unique<BimodalPredictor>(m2);
    }

    // Gshare: sim gshare <M1> <N> <tracefile>
    if (mode == "gshare")
    {
        if (args.size() != 2)
        {
            throw std::runtime_error("gshare mode requires 2 numeric arguments");
        }

        int m1 = std::stoi(args[0]);
        int n = std::stoi(args[1]);

        // Create and return a GsharePredictor object from gshare.h configured with the provided arguments.
        return std::make_unique<GsharePredictor>(m1, n);
    }

    // Hybrid: sim hybrid <K> <M1> <N> <M2> <tracefile>
    if (mode == "hybrid")
    {
        if (args.size() != 4)
        {
            throw std::runtime_error("hybrid mode requires 4 numeric arguments");
        }

        int k = std::stoi(args[0]);
        int m1 = std::stoi(args[1]);
        int n = std::stoi(args[2]);
        int m2 = std::stoi(args[3]);

        // Create and return a HybridPredictor object from hybrid.h configured with the provided arguments.
        return std::make_unique<HybridPredictor>(k, m1, n, m2);
    }

    throw std::runtime_error("unknown predictor mode: " + mode);
}
