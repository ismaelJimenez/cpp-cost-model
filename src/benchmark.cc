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

std::unordered_map<int, int> ConstructRandomUnorderedMap(int size) {
  std::unordered_map<int, int> m;
  m.reserve(size*1.3);
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
    c.reserve(state.range_x());
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

////////////////////////////////////  NUMERICS ////////////////////////////////////

static void BM_PowOfTwo(benchmark::State& state) {
  const int size = state.range_x()-1;
  while (state.KeepRunning())
    benchmark::DoNotOptimize(pow(2,size));
}
BENCHMARK(BM_PowOfTwo)
	->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64);

static void BM_PowOfTwoWithBitShift(benchmark::State& state) {
  const int size = state.range_x()-1;
  int64_t  z = 1;
  while (state.KeepRunning())
    benchmark::DoNotOptimize(z << size);
}
BENCHMARK(BM_PowOfTwoWithBitShift)
	->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64);

static void BM_IntMultiplication(benchmark::State& state) {
  int x = std::numeric_limits<int>::max()/3;
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(x*2);
  }
}
BENCHMARK(BM_IntMultiplication);

static void BM_IntDivision(benchmark::State& state) {
  int x = std::numeric_limits<int>::max();
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(x/2);
  }
}
BENCHMARK(BM_IntDivision);

static void BM_DoubleMultiplication(benchmark::State& state) {
  double d1 = std::numeric_limits<double>::max()/3;
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(d1*2);
  }
}
BENCHMARK(BM_DoubleMultiplication);

static void BM_DoubleDivision(benchmark::State& state) {
  double d1 = std::numeric_limits<double>::max();
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(d1/2.5);
  }
}
BENCHMARK(BM_DoubleDivision);

static void BM_ConvertIntegerToString(benchmark::State& state) {
  const int size = pow(10,state.range_x()-1);
  while (state.KeepRunning())
    	benchmark::DoNotOptimize(std::to_string(size));
}
BENCHMARK(BM_ConvertIntegerToString)
	->Arg(1)->Arg(2)->Arg(4)->Arg(8);

static void BM_ConvertFromStringToInt(benchmark::State& state) {
  std::string x1 = std::string(state.range_x(), '1');
  while (state.KeepRunning())
    	benchmark::DoNotOptimize(std::stoi(x1));
}
BENCHMARK(BM_ConvertFromStringToInt)
	->Arg(1)->Arg(2)->Arg(4)->Arg(8);

////////////////////////////////////  STRINGS ////////////////////////////////////

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
  std::string s1(state.range_x(), '-');
  std::string copy;
  while (state.KeepRunning())
    benchmark::DoNotOptimize(copy = s1);
}
BENCHMARK(BM_StringCopy)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000)->Arg(128000)->Arg(256000);

static void BM_StringCompare(benchmark::State& state) {
  std::string s1(state.range_x(), '-');
  std::string s2(state.range_x(), '-');
  while (state.KeepRunning())
    benchmark::DoNotOptimize(s1.compare(s2));
}
BENCHMARK(BM_StringCompare)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000)->Arg(128000)->Arg(256000);

static void BM_StringConcatenate(benchmark::State& state) {
  std::string s1(state.range_x(), '-');
  std::string s2(state.range_x(), '-');
  while (state.KeepRunning())
    benchmark::DoNotOptimize(s1+s2);
}

BENCHMARK(BM_StringConcatenate)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000)->Arg(128000)->Arg(256000);

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

BENCHMARK_DEFINE_F(VectorFixture, VectorReverse)(benchmark::State& state) {
  while (state.KeepRunning()) {
      std::reverse(v.begin(), v.end());
  }
}
BENCHMARK_REGISTER_F(VectorFixture, VectorReverse)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(VectorFixture, VectorSort)(benchmark::State& state) {
  while (state.KeepRunning()) {
      std::sort(v.begin(), v.end());
  }
}
BENCHMARK_REGISTER_F(VectorFixture, VectorSort)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(VectorFixture, VectorCopy)(benchmark::State& state) {
  while (state.KeepRunning()) {
      std::vector<int> copy = v;
      (void)copy;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(VectorFixture, VectorCopy)
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

BENCHMARK_DEFINE_F(MapFixture, MapCopy)(benchmark::State& state) {
  while (state.KeepRunning()) {
      std::map<int, int> copy = m;
      (void)copy;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(MapFixture, MapCopy)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);


////////////////////////////////////  UNORDERED MAPS ////////////////////////////////////

class UnorderedMapFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& st) {
    m = ConstructRandomUnorderedMap(st.range_x());
  }

  void TearDown(const ::benchmark::State&) {
    m.clear();
  }

  std::unordered_map<int, int> m;
};

static void BM_createEmptyUnorderedMap(benchmark::State& state) {
  while (state.KeepRunning())
  {
	std::unordered_map<int, int> m;
	(void)m;
  } 
}
BENCHMARK(BM_createEmptyUnorderedMap);

static void BM_SequentialUnorderedMap(benchmark::State& state) {
  std::pair<int, int> v = std::make_pair(42, 55);
  while (state.KeepRunning()) {
    std::unordered_map<int, int> c;
    for (int i = state.range_x(); --i; )
      c.insert(v);
  }
  const size_t items_processed = state.iterations() * state.range_x();
  state.SetItemsProcessed(items_processed);
  state.SetBytesProcessed(items_processed * sizeof(v));
}
BENCHMARK(BM_SequentialUnorderedMap)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(UnorderedMapFixture, UnorderedMapLookup)(benchmark::State& state) {
  const int itemNotInVector = state.range_x()*2; // Test worst case scenario (item not in vector)
  while (state.KeepRunning()) {
      benchmark::DoNotOptimize(m.find(itemNotInVector));
  }
}
BENCHMARK_REGISTER_F(UnorderedMapFixture, UnorderedMapLookup)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_DEFINE_F(UnorderedMapFixture, UnorderedMapCopy)(benchmark::State& state) {
  while (state.KeepRunning()) {
      std::unordered_map<int, int> copy = m;
      (void)copy;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(UnorderedMapFixture, UnorderedMapCopy)
	->Arg(1000)->Arg(2000)->Arg(4000)->Arg(8000)->Arg(16000)->Arg(32000)->Arg(64000);

BENCHMARK_MAIN()
