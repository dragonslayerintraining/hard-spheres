all: hard-disk-perfect-sampler test-finite-graph

.PHONY: clean
clean:
	rm hard-disk-perfect-sampler test-finite-graph

hard-disk-perfect-sampler: hard-disk-perfect-sampler.cpp randomness-recycler.hpp
	g++ -g -Wall -O2 -std=c++20 -o hard-disk-perfect-sampler hard-disk-perfect-sampler.cpp

test-finite-graph: test-finite-graph.cpp randomness-recycler.hpp
	g++ -g -Wall -O2 -std=c++20 -o test-finite-graph test-finite-graph.cpp
