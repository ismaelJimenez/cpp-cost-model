LOADLIBES = -lbenchmark -lpthread
CFLAGS = -Wall -O3 -std=c++14 -fno-omit-frame-pointer

benchmark: src/benchmark.cc
	g++ $(CFLAGS) -g -o benchmark src/benchmark.cc $(LOADLIBES)



