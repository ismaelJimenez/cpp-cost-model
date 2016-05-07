# C++ Cost-Model

# C++ Running Time Analysis

The experiments are runned for inputs of various size, and then the running time is found by a least-squares fit to find the coefficient for the high order term. 

The machine used was an Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz and 4GB RAM.

#### Cost of C++ Integer Operations
###### i1 is an n-bit numbers and s is an n-digit string
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| n-th power of two | pow(2, N) | 64 ns | N <= 63 | 1% |
| n-th power of two (bit shift) | 1 << N | less than 2 ns | N <= 63 | 1% |
| Convert integer to string  | to_string(i1) | 90 ns | N <= 8 | 7% |
| Convert string to integer  | stoi(s) | 23 ns | N <= 8 | 26% |
| Multiplication (int/double)  | x * y | less than 2 ns |  | 1% |
| Division (int/double)  | x / y | less than 2 ns |  | 1% |

#### Cost of C++ Vector Operations
###### v1 and v2 are length-n std::vectors<int> and v3 is an empty vector
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Create an empty vector | vector<int> v1 | less than 2 ns | N = 1 | 1% |
| Push back N items | v3.push_back(...) | 2.5 * N ns | N <= 64000 | 13% |
| Lookup | find(v1.begin(), v1.end(), item) | 0.38 * N ns | N <= 64000 | 18% |
| Access | v1.at(500) | less than 2 ns | N <= 64000 | 1% |

#### Cost of C++ Map Operations
###### m1 and m2 are length-n std::map<int, int> and m3 is an empty map
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Create an empty map | map<int, int> m1 | less than 2 ns | N = 1 | 1% |
| Insert N items | m3.insert(...) | 0.17 * N * lg(N) ns | N <= 64000 | 17% |
| Lookup | m1.find(...) | 0.78 * lg(N) ns | N <= 64000 | 20% |
