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

std::map<int, int> ConstructRandomMap(int size) {
  std::map<int, int> m;
  for (int i = 0; i < size; ++i) {
    m.insert(std::make_pair(rand() % size, rand() % size));
  }
  return m;
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

static void BM_Void(benchmark::State& state) {
  while (state.KeepRunning()) {}
}

////////////////////////////////////  VECTORS ////////////////////////////////////

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
BENCHMARK(BM_Void);

static void BM_createEmptyVector(benchmark::State& state) {
  while (state.KeepRunning())
  {
	std::vector<int> v;
	(void)v;
  } 
}
BENCHMARK(BM_createEmptyVector);

BENCHMARK_DEFINE_F(VectorFixture, VectorFind)(benchmark::State& state) {
  const int itemNotInVector = state.range_x()*2; // Test worst case scenario (item not in vector)
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(std::find(v.begin(), v.end(), itemNotInVector));
  }
}
BENCHMARK_REGISTER_F(VectorFixture, VectorFind)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(VectorFixture, VectorLookup)(benchmark::State& state) {
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(v.at(500));
  }
}
BENCHMARK_REGISTER_F(VectorFixture, VectorLookup)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_TEMPLATE2(BM_Sequential, std::vector<int>, int)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

////////////////////////////////////  MAPS ////////////////////////////////////

class MapFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& st) {
    m = ConstructRandomMap(st.range_x());
  }

  void TearDown(const ::benchmark::State&) {
    m.clear();
  }

  std::map<int, int> m;
};

static void BM_createEmptyMap(benchmark::State& state) {
  while (state.KeepRunning())
  {
	std::map<int, int> m;
	(void)m;
  } 
}
BENCHMARK(BM_createEmptyMap);

BENCHMARK_DEFINE_F(MapFixture, MapLookup)(benchmark::State& state) {
  const int itemNotInVector = state.range_x()*2; // Test worst case scenario (item not in vector)
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(m.find(itemNotInVector));
  }
}
BENCHMARK_REGISTER_F(MapFixture, MapLookup)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

static void BM_SequentialMap(benchmark::State& state) {
  std::pair<int, int> v = std::make_pair(42, 55);
  while (state.KeepRunning()) {
    std::map<int, int> c;
    for (int i = state.range_x(); --i; )
      c.insert(v);
  }
  const size_t items_processed = state.iterations() * state.range_x();
  state.SetItemsProcessed(items_processed);
  state.SetBytesProcessed(items_processed * sizeof(v));
}
BENCHMARK(BM_SequentialMap)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_MAIN()
