# C++ Cost-Model

# C++ Running Time Analysis

The experiments are executed for inputs of various size, and then the running time is found by a least-squares fit to find the coefficient for the high order term. 

The machine used was an Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz and 4GB RAM. 

Results may differ from the table below due to random run-time variations.

#### Cost of C++ Numeric Operations
###### z is an n-bit numbers and s is an n-digit string
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| n-th power of two | pow(2, N) | 64 ns | N <= 63 | 1% |
| n-th power of two (bit shift) | 1 << N | less than 2 ns | N <= 63 | 1% |
| Convert integer to string  | to_string(z) | 90 ns | N <= 8 | 7% |
| Convert string to integer  | stoi(s) | 23 ns | N <= 8 | 26% |
| Multiplication (int/double)  | x * y | less than 2 ns |  | 1% |
| Division (int/double)  | x / y | less than 2 ns |  | 1% |

#### Cost of C++ String Operations
###### s and t are length-n strings
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Copy | s = t | 0.0284708 * N ns | N <= 256000 | 30% |
| Compare | s.compare(t) | 0.0344692 * N ns | N <= 256000 | 15% |
| Concatenate | s + t | 0.133415 * N ns | N <= 256000 | 31% |

Notes:
*  String comparison complexity is linear in both the compared and comparing string's lengths. So, as a general case it could also be though as (0.0172346 * (size of s + size of t))
*  String concatenation complexity is linear in the resulting string length. So, as a general case it could also be though as (0.0667073 * (size of s + size of t))

#### Cost of C++ Vector Operations
###### v1 and v2 are length-n std::vectors< int > and v3 is an empty vector
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Create an empty vector | vector<int> v1 | less than 2 ns | N = 1 | 1% |
| Push back | v3.push_back(...) | 2.3 ns | N <= 64000 | 8% |
| Lookup | find(v1.begin(), v1.end(), item) | 0.5 * N ns | N <= 64000 | 8% |
| Access | v1.at(500) | less than 2 ns | N <= 64000 | 1% |
| Reverse | reverse(v1.begin(), v1.end()) | 0.8 * (N/2) ns | N <= 64000 | 7% |
| Sort | sort(v1.begin(), v1.end()) | 1.01305 * N * lg(N) ns | N <= 64000 | 3% |
| Copy | v2 = v1 | 0.15 * N ns | N <= 64000 | 16% |

#### Cost of C++ Map Operations
###### m1 and m2 are length-n std::map< int, int > and m3 is an empty map
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Create an empty map | map<int, int> m3 | less than 2 ns | N = 1 | 1% |
| Insert N items | m3.insert(...) | 0.17 * N * lg(N) ns | N <= 64000 | 17% |
| Lookup | m1.find(...) | 0.86 * lg(N) ns | N <= 64000 | 6% |
| Copy | m2 = m1 | 33.4 * N ns | N <= 64000 | 17% |

Notes:
* Complexity of map insert is N * log(size+N). As test was performed with an initially empty map, complexity obtained is N * log(N)

#### Cost of C++ Unordered Map Operations
###### m1 and m2 are length-n std::unordered_map< int, int > and m3 is an empty unordered map
| Operation | Command | Running Time | Size | RMS error |
| --- | --- | --- | --- | --- |
| Create an empty unordered map |  unrodered_map<int, int> m3 | less than 2 ns | N = 1 | 1% |
| Insert | m3.insert(...) | 35.7 ns | N <= 64000 | 13% |
| Lookup | m1.find(...) | 17 ns | N <= 64000 | 15% |
| Copy | m2 = m1 | 31.7 * N ns | N <= 64000 | 7% |

## Debug vs Release
By default, benchmark builds as a debug library. You will see a warning in the output when this is the case. To build it as a release library instead, use:

```
cmake -DCMAKE_BUILD_TYPE=Release
