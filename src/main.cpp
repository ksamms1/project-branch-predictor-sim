// main.cpp
// Entry point for the branch predictor simulator.
// This version validates arguments, creates the predictor object,
// reads the trace file, runs the simulation loop, and prints
// basic results plus the predictor's final contents.

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "single_program_sim.h"

// Print the accepted command formats.
// <M2> is the number of bits for the bimodal branch predictor index.
// <M1> is the number of bits for the gshare branch predictor index.
// <N> is the number of bits in the global history register for gshare.
// <K> is the number of bits for the hybrid branch's chooser index (to choose between predictors).
static void print_usage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "  sim smith <counter_bits> <tracefile>" << std::endl;
    std::cout << "  sim bimodal <M2> <tracefile>" << std::endl;
    std::cout << "  sim gshare <M1> <N> <tracefile>" << std::endl;
    std::cout << "  sim hybrid <K> <M1> <N> <M2> <tracefile>" << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        // Need at least program name + mode + one more argument.
        if (argc < 3)
        {
            print_usage();
            return 1;
        }

        // Predictor mode is the first user argument.
        std::string mode = argv[1];

        // Collect all remaining arguments after the mode.
        std::vector<std::string> args;
        for (int i = 2; i < argc; i++)
        {
            args.push_back(argv[i]);
        }

        // Validate expected argument count for each mode.
        // smith requires 1 predictor argument + tracefile
        if (mode == "smith")
        {
            if (args.size() != 2)
            {
                print_usage();
                return 1;
            }
        }
        // bimodal requires 1 predictor argument + tracefile
        // argument is M2 = number of PC bits for indexing into the predictor table of 3-bit counters
        else if (mode == "bimodal")
        {
            if (args.size() != 2)
            {
                print_usage();
                return 1;
            }
        }
        // gshare requires 2 predictor arguments + tracefile
        // arguments are M1 and N 
        // M1 = number of PC bits for indexing into the predictor table of 3-bit counters
        // N = number of bits in the global history register to XOR with the PC bits for indexing into the predictor table of 3-bit counters
        else if (mode == "gshare")
        {
            if (args.size() != 3)
            {
                print_usage();
                return 1;
            }
        }
        // hybrid requires 4 predictor arguments + tracefile
        // arguments are K, M1, N, M2
        // K = number of bits for the chooser index (to choose between gshare and bimodal)
        // M1 and N are for the gshare predictor (M1 = number of PC bits for indexing into the gshare predictor, 
        //        N = number of bits in the global history register to XOR with the PC bits for indexing into the gshare predictor's table of 3-bit counters)
        // M2 is for the bimodal predictor (M2 = number of PC bits for indexing into the bimodal predictor's table of 3-bit counters)
        else if (mode == "hybrid")
        {
            if (args.size() != 5)
            {
                print_usage();
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: unknown mode: " << mode << std::endl;
            print_usage();
            return 1;
        }

        // The last argument is always the trace file.
        std::string tracefile = args.back();

        // All earlier arguments are numeric predictor configuration arguments.  (-1 to exclude the tracefile)
        std::vector<std::string> predictor_args(args.begin(), args.end() - 1);

        // Run the single-program simulation with the given mode, predictor arguments, and tracefile.
        return run_single_program_sim(mode, predictor_args, tracefile);
    }
    catch (const std::exception& ex)    
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}