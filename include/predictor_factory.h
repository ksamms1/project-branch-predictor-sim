#ifndef PREDICTOR_FACTORY_H
#define PREDICTOR_FACTORY_H

// predictor_factory.h
// Declares a helper function that creates the correct predictor object
//   based on command-line mode and parameters.
// This allows any .cpp file with the function signature and header file import to call create_predictor() 
//   in main.cpp without needing to know the details of each predictor class.

#include <memory>
#include <string>
#include <vector>

#include "predictor_base.h"

// Create and return the correct predictor object.
// mode = smith, bimodal, gshare, or hybrid
// args = numeric arguments associated with that mode
std::unique_ptr<PredictorBase> create_predictor(
    const std::string& mode,
    const std::vector<std::string>& args
);

#endif
