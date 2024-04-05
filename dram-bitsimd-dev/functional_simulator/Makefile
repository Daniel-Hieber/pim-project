# Author: Deyuan Guo

.PHONY: all clean

CC = g++
LIBS =
CFLAGS = -Wall -std=c++17 -O3

SRCS = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
OBJS = $(SRCS:.cc=.out)

%.out: %.cc $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

all: $(OBJS)

clean:
	rm -rf *.out

