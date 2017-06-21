# Makefile that builds the Signal unit tests

# Compiler:
CC=g++

# Compiler options:
CFLAGS=-Wall --std=c++11

unit_test: Signal.h signal_ut.cpp
	$(CC) $(CFLAGS) signal_ut.cpp -o $@

.PHONY: clean
clean:
	rm unit_test
