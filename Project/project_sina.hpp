/*
Project
Created by: Sina Ghanadian
Submitted on: 2023-03-29
*/
#ifndef PROJECT_HPP //Include guard for project.hpp
#define PROJECT_HPP

#include "utils.hpp"
#include <iostream>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <ranges>
#include <random>
#include <string>
#include <limits>
#include <cctype>
#include <iterator>

#include "utils.hpp"

/*
Using examples from cpp reference 
https://en.cppreference.com/w/cpp/algorithm/min
*/

//Namespace provided in assignment instructions

namespace uwindsor_2023w::comp3400::project {

/*
Min Overloaded Definitions

Prototypes for min provided in assignment instructions
*/
template <typename T>
constexpr T const& min(T const& a) {
    return a; //returns a
}


template <typename T>
constexpr T const& min(T const& a, T const& b) {
    return (b < a) ? b : a; // min(1) from cppreference
}

template <typename T, typename... Rest>
requires (std::same_as<T,Rest> && ...)
constexpr T const& min(T const& a, T const& b, Rest const&... rest) 
{
    return min(min(a, b), rest...); //Uses recursion to compare a and b then compares with rest
} 

/*
Levenshtein Function Definition

function prototype provided in assignment
*/


    template <typename StringA, typename StringB>
requires
    std::ranges::sized_range<StringA> &&
    std::ranges::sized_range<StringB> &&
    std::ranges::random_access_range<StringA> &&
    std::ranges::random_access_range<StringB> &&
    std::same_as<
        std::ranges::range_value_t<StringA>,
        std::ranges::range_value_t<StringB>
    >
    
std::size_t levenshtein(StringA const& a, StringB const& b) {

    /*
    From cppreference std::ranges library
    https://en.cppreference.com/w/cpp/ranges

    From cppreference std::size_t library
    https://en.cppreference.com/w/cpp/types/size_t
    I used size_t to store the size of a character

    From cppreference std::iota library
    https://en.cppreference.com/w/cpp/algorithm/iota

    From cppreference std::swap library
    https://en.cppreference.com/w/cpp/algorithm/swap

    Levenshtein function pseudocode from
    https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
    */
    const std::size_t m = std::ranges::size(a); // String a has m elements
    const std::size_t n = std::ranges::size(b); // String b has n elements
    //Creates two vectors
    std::vector<std::size_t> s0(n + 1);
    std::vector<std::size_t> s1(n + 1);

    /*
    In our code we use std::iota to initialize a range
    This takes two iterators which are the 
    start and end of the range.

    This acts as the initial for loop in the pseudocode as it is more efficient
    */

    std::iota(std::ranges::begin(s0), std::ranges::end(s0), 0);

    //Calculates s1 row distances
    for (std::size_t i = 0; i < m; ++i) {
        s1[0] = i + 1;

        for (std::size_t j = 0; j < n; ++j) { 
            const std::size_t deletionCost = s0[j + 1] + 1;
            const std::size_t insertionCost = s1[j] + 1;
            std::size_t substitutionCost;
            if(a[i] == b[j]){
                substitutionCost = s0[j];
            } else {
                substitutionCost = s0[j] + 1;
            }
            s1[j + 1] = std::min({deletionCost, insertionCost, substitutionCost});
        }

        std::swap(s0, s1); 
    }
    
    return s0[n];
}

/*
Mutator

std::to_string referenced from cppreference
https://en.cppreference.com/w/cpp/string/basic_string/to_string
*/


class char_mutator {
private:
    std::string valid;
    mutable std::uniform_int_distribution<std::size_t> uni_int_dist;
    mutable std::default_random_engine rand_engine;
public:
    template<typename... Args> //Member function with template parameter
    char operator()(Args&&...) const 
    {
        return valid[uni_int_dist(rand_engine)]; //returns the index
    }
    char_mutator() : valid([]()
    {
        std::string retval;
        for (short i = 0; i <= std::numeric_limits<char>::max(); ++i) {
            if (std::isalnum(i) || std::ispunct(i) || i == ' ') {
                retval += std::to_string(i); //Appending value of i to retval
            }
        }
        return retval; //Returns value of retval
    }()), uni_int_dist(0, valid.size() - 1), rand_engine(std::random_device{}()) {} //Uniform distribution member variable generating
};

template <std::ranges::range Individual, typename MutateOp, typename URBG>
requires std::uniform_random_bit_generator<std::remove_cvref_t<URBG>> &&
         std::invocable<MutateOp, std::ranges::range_value_t<Individual>>
void mutate(
    Individual& individual, 
    double const rate, 
    MutateOp&& m, 
    URBG&& urbg)
{
    std::uniform_real_distribution<double> uni_int_dist(0.0, 1.0); //declare uniform real distribution
    std::ranges::for_each(individual, [&](auto& arg) { //Received the arg argument
        auto random = uni_int_dist(urbg);
        if (random < rate) {
            arg = m(arg); //Assigns arg to m(arg)
        }
    });
}

/*
Crossover function

*/

template <typename URBG1, typename URBG2, typename Individual>
requires
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG1>> &&
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG2>> &&
  std::ranges::forward_range<Individual> &&
  std::ranges::sized_range<Individual> &&
  uwindsor_2023w::comp3400::project::smart_insertable<Individual>
auto crossover(
    std::size_t const ncrossover_points,
    URBG1&& urbg_starting_parent,
    URBG2&& urbg_crossover_points,
    Individual const& parent1,
    Individual const& parent2
) -> std::remove_cvref_t<Individual> 
{
    //Declaring begin and end
    using std::ranges::begin;
    using std::ranges::end;
    
    
    auto const psize = std::min(std::ranges::size(parent1), std::ranges::size(parent2)); //The minimum size of the parent
    std::vector<std::size_t> crossover_indices(ncrossover_points);
    std::generate(std::begin(crossover_indices), std::end(crossover_indices), [&]() {
    return std::uniform_int_distribution<std::size_t>{0, psize}(urbg_crossover_points);
    });
    std::ranges::sort(crossover_indices);

    Individual retval;
    retval.reserve(psize);

    auto const starting_parent = [&]() -> Individual const& {
        return (std::uniform_int_distribution<std::size_t>(0, 1)(urbg_starting_parent) == 0) ? parent1 : parent2;
    }();

    auto next_crossover_point = std::begin(crossover_indices);
    auto it_parent = begin(starting_parent);
    auto const end_parent = end(starting_parent);
    for (std::size_t i = 0; i < psize; ++i, ++it_parent) {
        if (it_parent == end_parent) {
            it_parent = begin((starting_parent == parent1) ? parent2 : parent1);
            retval.push_back(*it_parent);
            continue;
        }
        if (next_crossover_point != std::end(crossover_indices) && i == *next_crossover_point) {
            it_parent = begin((it_parent == begin(starting_parent)) ? parent2 : parent1);
            ++next_crossover_point;
        }
        retval.push_back(*it_parent);
    }

    return retval;
    
}

}

#endif // PROJECT_HPP