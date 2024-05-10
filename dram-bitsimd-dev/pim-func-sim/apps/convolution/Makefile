# Makefile: C++ version of vector arithmetic
# Copyright 2024 LavaLab @ University of Virginia. All rights reserved.

CXX := g++
CXXFLAGS := -std=c++17 -O3
INCLUDE_DIRS := -I../../libpimsim/include
LIB_DIRS := -L../../libpimsim/lib
LIBS := -lpimsim
EXEC := conv
SRC := conv.cpp
LIB := libpim.so
LIB_SRCS := $(wildcard ../../libpimsim/src/*.cpp)

.PHONY: all clean $(INCLUDE_DIRS)/libpimsim.h $(LIB_DIRS)/libpimsim.a

all: $(EXEC) $(LIB)
	@echo "Compilation successful"

$(EXEC): $(SRC) $(INCLUDE_DIRS)/libpimsim.h $(LIB_DIRS)/libpimsim.a
	@echo "Compiling $<"
	$(CXX) $< $(CXXFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -o $@

$(LIB): $(SRC) $(LIB_SRCS) $(INCLUDE_DIRS)/libpimsim.h $(LIB_DIRS)/libpimsim.a
	@echo "Compiling $< into $@"
	$(CXX) $(CXXFLAGS) $(LIB_SRCS) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -fPIC -shared -o $@ $< 
clean:
	rm -f $(EXEC) $(LIB)