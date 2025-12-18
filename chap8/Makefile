# Copyright (c) Alexander Matzen. All rights reserved.
# Author: Alexander Matzen
# Licensed under the MIT license

# Makefile for Chapter 8 Exercises
# Usage:
#   make exercise1
#   make exercise4
#   make exercise6
#   make exercise10
#   make student_grades
#   make all
#   make clean

CXX = g++
CXXFLAGS = -std=c++11 -Wall

EXE1 = exercise1
EXE2 = exercise4
EXE3 = exercise6
EXE4 = exercise10
EXE5 = student_grades

all: $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5)

$(EXE1): exercise1.cpp
	$(CXX) $(CXXFLAGS) -o $(EXE1) exercise1.cpp

$(EXE2): exercise4.cpp
	$(CXX) $(CXXFLAGS) -o $(EXE2) exercise4.cpp

$(EXE3): exercise6.cpp
	$(CXX) $(CXXFLAGS) -o $(EXE3) exercise6.cpp

$(EXE4): exercise10.cpp
	$(CXX) $(CXXFLAGS) -o $(EXE4) exercise10.cpp

$(EXE5): student_grades.cpp
	$(CXX) $(CXXFLAGS) -o $(EXE5) student_grades.cpp

clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5)
