# benchmarkParser.py
# Author: Ismael Jimenez
# Date last modified: May 6, 2016

# Routines to help in parse the timing results of
# various code fragments or routines, and to
# infer a good formula for the resulting runtimes.

#!/usr/bin/python

import sys, getopt
import math
import scipy.linalg
import string
import timeit

class output_colors:
    header = '\033[32m'
    rms = '\033[33m'
    fit = '\033[36m'
    fail = '\033[91m'
    endc = '\033[0m'

def usage():
    """
    Prints the usage of the script and ends program execution.
    Example:
       usage()
    """
    print 'usage : parse.py -i <inputfile>'
    sys.exit(2)
    return

def print_test_results(header, test_name, fit_list, tests_result, growth_factor = 2):
    """
    Print results for Benchmark 'test_name'.
    Inputs:
        header       : custom header for the benchmark test
        test_name    : name of the benchmark test, as defined on 'benchmark.cc'
        fit_list     : list of functions to be tested by least squares fit
        tests_result :list of all test results
    """
    print
    print output_colors.header + header + output_colors.endc
    results_list = [item for item in tests_result if item[0] == test_name] # Get all results for current test
    n_range = [x[1] for x in results_list]                                 # Get range tested for current test

    if(len(n_range) > 1):
        input_range = n_range[0]+"<=n<="+n_range[len(n_range)-1]    # Input range string to be sent to generate_parameters(...)
    elif(len(n_range) == 1):
        input_range = "1<=n<=1"                                     # Current benchmark is a single test
    else:
        print output_colors.fail + "Test not found on results list!" + output_colors.endc
        return # Test not found on list

    var_list, param_list = generate_parameters(input_range, growth_factor)
    print_fit_coefficients(var_list, param_list, results_list, fit_list)

def lg(x):
    return math.log(x)/math.log(2.0)

def sqrt(x):
    return math.sqrt(x)

def generate_parameters(input_range,growth_factor):
    """
    Generate a list of dictionaries given maximum and minimum range values.
    Values increment by factor of growth_factor from min to max.
    Example:
       generate_parameters("1000<=n<=64000")
    """
    var_list = []       
    spec_list = string.split(input_range,";")
    D = {}
    D['lg']=lg
    D['sqrt'] = sqrt
    D_list = [D]
    for spec in spec_list:
        spec_parts = string.split(spec,"<=")
        assert len(spec_parts)==3
        lower_spec = spec_parts[0]
        var_name = spec_parts[1]
        assert len(var_name)==1
        var_list.append(var_name)
        upper_spec = spec_parts[2]
        new_D_list = []
        for D in D_list:
            new_D = D.copy()
            val = eval(lower_spec,D)
            while val<=eval(upper_spec,D):
                new_D[var_name] = val
                new_D_list.append(new_D.copy())
                val *= growth_factor
        D_list = new_D_list
    return (var_list,D_list)

def print_fit_coefficients(var_list, param_list, results_list, fit_list):
    """
    Print least-squares fit coefficients for each function in fit_list
    Inputs:
        var_list      : list of variable names
        param_list    : list of sample dicts for various parameter sets
        results_list : list of corresponding run times
        fit_list        :list of functions to be considered for fit, e.g. "n","n**2",etc.
    """
    print "Function list :",fit_list
    print "Benchmark run times :",

    run_times = [x[2] for x in results_list]   # Get runtimes list for current test
    for i in range(len(param_list)):           # Print runtimes for current test
        print
        for v in var_list:
            print "  ",v,"= %6s"%param_list[i][v],
        print ": %6s"%run_times[i],"nanoseconds",
    print
    rows = len(run_times)
    cols = len(fit_list)
    A = [ [0 for j in range(cols)] for i in range(rows) ]
    for i in range(rows):       # Evaluate fit list parameters
        D = param_list[i]
        for j in range(cols):
            A[i][j] = float(eval(fit_list[j],D))
    b = run_times

    (x,resids,rank,s) = relative_error_minimizer(A,b)

    for j in range(cols):        # Print cost of the current operation tested
        sign = ''
        if x[j]>0 and j>0: 
            sign="+"
        elif x[j]>0:
            sign = " "
        print "Cost of operation : "+output_colors.fit + "%s%g*%s"%(sign,x[j],fit_list[j]) + output_colors.endc + " (time measured in nanoseconds)"

    print "RMS error         :  "+ output_colors.rms +"{0:.0f}%".format(math.sqrt(resids/len(A))*100.0) + output_colors.endc
    sys.stdout.flush()    
import scipy.optimize

def relative_error_minimizer(A,b):
    """ Relative error minimizer """
    def f(x):
        assert len(x) == len(A[0])
        resids = []
        for i in range(len(A)):
            sum = 0.0
            for j in range(len(A[0])):
                sum += A[i][j]*x[j]
            relative_error = (sum-b[i])/b[i]
            resids.append(relative_error)
        return resids
    ans = scipy.optimize.leastsq(f,[0.0]*len(A[0]))
    if len(A[0])==1:
        x = [ans[0]]
    else:
        x = ans[0]
    resids = sum([r*r for r in f(x)])
    return (x,resids,0,0)

def readInput(argv):
    inputfile = ''
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:",["help", "input="])
    except getopt.GetoptError:
        usage()

    for opt, arg in opts:
        if opt in ("-i", "--ifile"):
            inputfile = arg
        else:
            usage()
	
    if inputfile == '':
        usage()

    f = open(inputfile)
    tests_result = []
    for line in iter(f):
        words = line.split()

        if len(words) >= 6:
            splitOfWordsAndSize = words[0].split("/") # Split the word into function name and size (if any)
		
            if(len(splitOfWordsAndSize) == 3):  # Check if current benchmark is a fixture
                method = splitOfWordsAndSize[1] # Get method name
                size =   splitOfWordsAndSize[2] # Get size tested ('N')
                splitOfSize = size.split("k")   # Split size into number and 'k' symbol (if any)

                if len(splitOfSize) == 1:       # Adapt size and add to test results
                    tests_result.append((method, size, float(words[2])))
                elif len(splitOfSize) == 2:
                    tests_result.append((method,str(float(splitOfSize[0])*1024), float(words[2])))
            elif(len(splitOfWordsAndSize) == 2):     # Check if current benchmark has size attacched and is not a fixture
                method = splitOfWordsAndSize[0][10:] # Get method name and remove all formating symbols
                size =   splitOfWordsAndSize[1]      # Get size tested ('N')
                splitOfSize = size.split("k")        # Split size into number and 'k' symbol (if any)
				
                if len(splitOfSize) == 1:            # Adapt size and add to test results
                    tests_result.append((method, size, float(words[2])))
                elif len(splitOfSize) == 2:
                    tests_result.append((method,str(float(splitOfSize[0])*1024), float(words[2])))
            else:
                method = splitOfWordsAndSize[0][10:] # Get method name and remove all formating symbols
                tests_result.append((method, 1, float(words[2]))) # Add to test results as single test
    f.close()
    return (tests_result)

def main(argv):
	tests_result = readInput(argv)
        #test_number(tests_result)
	test_vector(tests_result)
        #test_map(tests_result)

def test_number(tests_result):
    print_test_results(
        "Number Benchmark 1 : Time to compute 2**N with pow",
        "BM_PowOfTwo",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 2 : Time to compute 2**N with bit shift",
        "BM_PowOfTwoWithBitShift",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 3 : Time to multiplicate two integers",
        "BM_IntMultiplication",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 4 : Time to divide two integers",
        "BM_IntDivision",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 5 : Time to multiplicate two doubles",
        "BM_DoubleMultiplication",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 6 : Time to divide two doubles",
        "BM_DoubleDivision",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 5 : Convert string to integer",
        "BM_ConvertFromStringToInt",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Number Benchmark 6 : Convert integer to string",
        "BM_ConvertIntegerToString",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)


def test_vector(tests_result):
    print_test_results(
        "Vector Benchmark 1 : Create an empty vector",
        "BM_createEmptyVector",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Vector Benchmark 2 : Push back N items",
        "BM_Sequential<std::vector<int>,int>",
        ("n",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Vector Benchmark 3 : Find an element on a vector of size N",
        "VectorFind",
        ("n",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Vector Benchmark 4 : Lookup on a vector of size N",
        "VectorLookup",
        ("1",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Vector Benchmark 5 : Reverse a vector of size N",
        "VectorReverse",
        ("n",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Vector Benchmark 6 : Sort a vector of size N",
        "VectorSort",
        ("n*lg(n)",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

def test_map(tests_result):
    print_test_results(
        "Map Benchmark 1 : Create an empty map",
        "BM_createEmptyMap",
        ("1",),               # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Map Benchmark 2 : Insert N items",
        "BM_SequentialMap",
        ("n*lg(n)",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

    print_test_results(
        "Map Benchmark 3 : Lookup on a map of size N",
        "MapLookup",
        ("lg(n)",),         # ("n**2","n*lg(n)","n","1")
        tests_result)

if __name__ == "__main__":
	main(sys.argv[1:])






