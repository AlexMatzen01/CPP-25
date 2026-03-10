# Makefile for CPP-25 chap10 exercises

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# List of source files
SOURCES = exercise11.cpp exercise12.cpp exercise15.cpp forensics_tool.cpp

# Generate executable names by removing .cpp
EXES = $(SOURCES:.cpp=)

all: $(EXES)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(EXES)
