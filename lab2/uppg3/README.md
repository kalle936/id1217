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
number of threads wanted.

Example = NT=4 make.
