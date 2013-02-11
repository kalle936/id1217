/**
 * C++ implementation of the Lab2 Assignment 3 Find Palindromic Words (40 points).
 *
 * Finds palindromic words in the dictionary specified by WORDSFILE.
 * Uses c++11 specific containers because unordered_set uses buckets
 * that is filled using the hash value whereas set uses binary search.
 * Lookup in a unordered_set is O(1) and lookup in a set is O(log(n)).
 *
 * One unordered_set is used to contain every word in the dictionary and one
 * unordered_set is used to hold the palindromic words found.
 *
 * The palindromic words are stored in a unordered_set because io is specified to be
 * performed sequentially and insertion in a unordered_set is O(1).
 *
 * Compile using make by either using make palindromic.out or using default rule run
 * which compiles (if needed) and then runs the program.
 *
 * Run it through default make rule run. Specify number of threads by defining NT as the
 * number of threads wanted. The output file can also be specified by defining FP as the
 * path to the desired file.
 *
 * Example =    "NT=4 make"
 *              "FP=output NT=8 make"
 */

#include <omp.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

#define WORDSFILE "words" /* Path to dictionary. */

/**
 * Returns a copy the input reversed.
 *
 * Example: in = "Hello"; out = "olleH"
 *
 * This takes the input parameter as a copy.
 * This means that the original is unaffected by the call.
 * (if you call get_reverse(input[i]) input[i] will not
 * change)
 */
std::string get_reverse(std::string in)
{
    std::reverse(in.begin(), in.end());
    return in;
}

int main(int argc, char ** argv)
{
    /* Opens the output file. */
    std::ofstream output;
    if(argc > 1)
    {
        output.open(argv[1]);
    }
    else
    {
        printf("Usage: ./palindromic.out OUTPUT_FILE\n");
        return EXIT_SUCCESS;
    }
    /* Opens the dictionary. */
    std::ifstream words(WORDSFILE);

    /* Read input and store in a vector as well as in a unordered_set. */
    std::vector<std::string> input;
    std::unordered_set<std::string> all_words;
    /* All the palindromic words will be stored here. */
    std::unordered_set<std::string> palindromic;
    std::string line; /* String used to store the read line. */
    if(words.is_open())
    {
        while(words.good())
        {
            getline(words,line); /* Read line. */
            /* Inserts it into the vector and the unordered_set. */
            input.push_back(line);
            all_words.insert(line);
        }
        words.close();
    }
    int i; /* Index for iterating over the vector with words. */
    double start_time, end_time; /* Used for timing. */
    start_time = omp_get_wtime(); /* Time just before parallel work start. */
#pragma omp parallel for schedule(static)
    for(i = 0; i < input.size(); ++i)
    {
        /* If the reversed word exist in the unordered_set of all words it is palindromic. */
        if(all_words.find(get_reverse(input[i])) != all_words.end())
        {
            /* Unordered_set is unfortunately not thread safe such as Microsoft's concurrent_unordered_set. */
#pragma omp critical(insertion_palindromic)
            {
                palindromic.insert(input[i]);
            }
        }
    }
    end_time = omp_get_wtime(); /* Time it once again after the parallel work has been done. */

    /* Writes to the output file. */
    for(auto it = palindromic.begin(); it != palindromic.end(); ++it)
    {
        output << *it << std::endl;
    }
    /* Close the file again. */
    output.close();
    /* Prints information about number of found palindromic words and parallel execution time. */
    std::cout << "Found " << palindromic.size() << " words in " << end_time - start_time << "." << std::endl;
    return EXIT_SUCCESS; /* EXIT_SUCCESS (0) to prevent anything else than 0 being returned upon successful execution,
                            make will read return value and report anything other than EXIT_SUCCESS (0). */
}
