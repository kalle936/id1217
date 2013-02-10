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
 * number of threads wanted.
 *
 * Example = NT=4 make.
 */

#include <omp.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

#define WORDSFILE "words"

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

    /* Read input and store in a vector. */
    std::vector<std::string> input;
    std::unordered_set<std::string> all_words;
    std::unordered_set<std::string> palindromic;
    std::string line;
    if(words.is_open())
    {
        while(words.good())
        {
            getline(words,line);
            input.push_back(line);
            all_words.insert(line);
        }
        words.close();
    }
    //std::cout << "Size of the dictionary is " << input.size() << "." << std::endl;
    //std::cout << "Size of the dictionary is " << all_words.size() << "." << std::endl;
    int i;
    double start_time, end_time;
    start_time = omp_get_wtime();
#pragma omp parallel for
    for(i = 0; i < input.size(); ++i)
    {
        if(all_words.find(get_reverse(input[i])) != all_words.end())
        {
#pragma omp critical
            {
                palindromic.insert(input[i]);
            }
        }
    }
    end_time = omp_get_wtime();

    for(auto it = palindromic.begin(); it != palindromic.end(); ++it)
    {
        output << *it << std::endl;
    }
    /* Close the file again. */
    output.close();
    std::cout << "Found " << palindromic.size() << " words in " << end_time - start_time << "." << std::endl;
    return EXIT_SUCCESS;
}
