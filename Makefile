# Makefile for branch predictor simulator
# This compiles all .cpp files into an executable named "sim"

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

TARGET = sim

SRC = \
	src/main.cpp \
	src/predictor_factory.cpp \
	src/trace_reader.cpp \
	src/predictors/smith.cpp \
	src/predictors/bimodal.cpp \
	src/predictors/gshare.cpp \
	src/predictors/hybrid.cpp \
	src/simulation/single_program_sim.cpp \
	src/simulation/multiprogram_sim.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
