//=============================================================================

#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include "project.hpp"

//=============================================================================

// 
// If ALTERNATIVE_CROSSOVER_IMPLEMENTATION is defined this program will 
// use the alternative definition of crossover() defined in
// "beyond_project.hpp". This crossover() code is much more generic/general
// than the code required in the project.
//
// This code demonstrates using crossover() where the parent ranges types
// differ (but have the same element type). Additionally the crossover()
// function makes use of an output iterator (whose value_type must be the
// same as the parent's element type) to write out the data. The latter
// allows the caller to control whether or not memory is allocated, etc.
// when writing the output. Additionally, this version of crossover() does
// not dynamically allocate any RAM in its implementation.
//

#define ALTERNATIVE_CROSSOVER_IMPLEMENTATION

#ifdef ALTERNATIVE_CROSSOVER_IMPLEMENTATION
  #include <forward_list>
  #include <list>
  #include "beyond_project.hpp"
#endif

//=============================================================================

int main()
{
  using namespace std;

#ifndef ALTERNATIVE_CROSSOVER_IMPLEMENTATION
  using uwindsor_2023w::comp3400::project::min;
  using uwindsor_2023w::comp3400::project::crossover;
#else
  using uwindsor_2023w::comp3400::beyond_project::min;
  using uwindsor_2023w::comp3400::beyond_project::crossover;
#endif

  std::ios_base::sync_with_stdio(false);
                       //123456789012345678901234567890123456789012345
  string const parent1{ "_________________________________________" };
  string const parent2{ "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" };
  cout 
    << "p1:\t" << quoted(parent1) << '\n'
    << "p2:\t" << quoted(parent2) << "\n\n"
  ;

#ifdef ALTERNATIVE_CROSSOVER_IMPLEMENTATION
  forward_list<char> const parent1_fl{parent1.begin(), parent1.end()};
  list<char> const parent2_l{parent2.begin(), parent2.end()};
#endif

  random_device rd;

  // Declare random engines for crossover()...
  default_random_engine which_parent_re{rd()};
  default_random_engine crossover_points_re{rd()};

  for (int i{}; i != 4; ++i)
  {
    for (int repeat{}; repeat != 8; ++repeat)
    {
#ifndef ALTERNATIVE_CROSSOVER_IMPLEMENTATION
      auto child = crossover(i, which_parent_re, crossover_points_re,
        parent1, parent2);
#else
      string child;
      child.reserve(max(parent1.size(),parent2_l.size()));
      crossover(i, which_parent_re, crossover_points_re, parent1_fl, parent2_l,
        back_inserter(child));
#endif
      cout << i << ":\t" << quoted(child) << '\n';
    }
    cout << '\n';
  }

  // set up random engine and distribution for the number of crossover
  // points...
  default_random_engine num_crossovers_re{rd()};
  uniform_int_distribution<size_t> num_crossovers_dist(
    0, min(parent1.size(), parent2.size())
  );

  for (int i{}; i != 20; ++i)
  {
    auto const n = num_crossovers_dist(num_crossovers_re);
#ifndef ALTERNATIVE_CROSSOVER_IMPLEMENTATION
    auto child = crossover(n, which_parent_re, crossover_points_re,
      parent1, parent2);
#else
      string child;
      child.reserve(max(parent1.size(),parent2_l.size()));
      crossover(n, which_parent_re, crossover_points_re, parent1_fl, parent2_l,
        back_inserter(child));
#endif
    cout << n << ":\t" << quoted(child) << '\n';
  }
}

//=============================================================================
