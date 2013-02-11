C++ implementation of the Lab2 Assignment 3 Find Palindromic Words (40 points).

Finds palindromic words in the dictionary specified by WORDSFILE.
Uses c++11 specific containers because unordered_set uses buckets
that is filled using the hash value whereas set uses binary search.
Lookup in a unordered_set is O(1) and lookup in a set is O(log(n)).

One unordered_set is used to contain every word in the dictionary and one
unordered_set is used to hold the palindromic words found.

The palindromic words are stored in a unordered_set because io is specified to be
performed sequentially and insertion in a unordered_set is O(1).

Compile using make by either using make palindromic.out or using default rule run
which compiles (if needed) and then runs the program.

Run it through default make rule run. Specify number of threads by defining NT as the
number of threads wanted. The output file can also be specified by defining FP as the
path to the desired file.

Example =    "NT=4 make"
             "FP=output NT=8 make"

Evaluation (8 hardware threads @ 2.0GHz)

Using median time of 5 runs using the linux words dictionary (99172 words).
1 thread    =   0.022s
2 threads   =   0.013s
4 threads   =   0.0077s
8 threads   =   0.0056s

Using the same dictionary only this time the same dictionary several times.
This results in 10 412 955 words (only 99172 unique). (I used cat words >> words2
in a for loop)
1 thread    =   2.35s
2 threads   =   1.44s
4 threads   =   0.77s
8 threads   =   0.52s

When the payload is 100 times bigger the execution time follows almost perfectly
and is 100 times longer. Perfect example is when using 4 threads 
0.0077 * 100 = 0.77.

The speedup for the larger dictionary is
1 thread    =   1 (per definition this is the case)
2 threads   =   1.63
4 threads   =   3.05
8 threads   =   4.51

Efficiency (Speedup/Number of cores)
1 thread    =   1 (per definition this is the case)
2 threads   =   0.815
4 threads   =   0.7625
8 threads   =   0.56375

As we can see from the speedup it is not at all optimal (optimal would be when
the speed up == number of threads e.g. 2 threads -> optimal speedup is 2) and it
gets worse when more cores are used.

Efficiency tells the same tale but even clearer. Efficiency decreases as number of
threads used increases. The curve is sub-linear, which is to be expected.

Fun fact when dynamic scheduling is used the execution time is significantly hit.
This makes sense as the payload are about the same for every iteration in the loop
and hence static scheduling makes sense to use.
