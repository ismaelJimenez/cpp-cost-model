// benchmark.cc
// Author: Ismael Jimenez
// Date last modified: May 6, 2016

// Routines to time the execution of various code fragments or routines

#include "benchmark/benchmark_api.h"
#include <cmath>
#include <sstream> 
#include <vector>
#include <map>
#include <limits>
#include <unordered_map>
#include <algorithm>

namespace {

std::vector<int> ConstructRandomVector(int size) {
  std::vector<int> v;
  v.reserve(size);
  for (int i = 0; i < size; ++i) {
    v.push_back(rand() % size);
  }
  return v;
}

template<typename Container, typename ValueType = typename Container::value_type>
static void BM_Sequential(benchmark::State& state) {
  ValueType v = 42;
  while (state.KeepRunning()) {
    Container c;
    for (int i = state.range_x(); --i; )
      c.push_back(v);
  }
  const size_t items_processed = state.iterations() * state.range_x();
  state.SetItemsProcessed(items_processed);
  state.SetBytesProcessed(items_processed * sizeof(v));
}     

}  // namespace

// Using fixtures.
class VectorFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& st) {
    v = ConstructRandomVector(st.range_x());
  }

  void TearDown(const ::benchmark::State&) {
    v.clear();
  }

  std::vector<int> v;
};

static void BM_Void(benchmark::State& state) {
  while (state.KeepRunning()) {}
}
// Register the function as a benchmark
BENCHMARK(BM_Void);

static void BM_createEmptyVector(benchmark::State& state) {
  while (state.KeepRunning())
  {
	std::vector<int> v;
	(void)v;
  } 
}
// Register the function as a benchmark
BENCHMARK(BM_createEmptyVector);

BENCHMARK_DEFINE_F(VectorFixture, VectorFind)(benchmark::State& state) {
  const int itemNotInVector = state.range_x()*2; // Test worst case scenario (item not in vector)
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(std::find(v.begin(), v.end(), itemNotInVector));
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(VectorFixture, VectorFind)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(VectorFixture, VectorLookup)(benchmark::State& state) {
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(v.at(500));
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(VectorFixture, VectorLookup)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_TEMPLATE2(BM_Sequential, std::vector<int>, int)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_MAIN()
