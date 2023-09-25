//=============================================================================

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "project.hpp"

//=============================================================================

//
// The sample solution of project.hpp has alternative implementations of the
// levenshtein(a,b) function as follows:
//   * if !defined(ALTERNATIVE_LEVENSHTEIN_IMPLEMENTATION) compiles sample
//     solution code per project requirements
//   * if defined(ALTERNATIVE_LEVENSHTEIN_IMPLEMENTATION) the sample solution
//     code relaxes the following from project requirements:
//       - arguments a and b are relaxed from being random-access ranges to
//         forward ranges
//       - the requirement of std::ranges::sized_range on arguments a and b
//         are removed
//
#define ALTERNATIVE_LEVENSHTEIN_IMPLEMENTATION

#ifdef ALTERNATIVE_LEVENSHTEIN_IMPLEMENTATION
  #include <forward_list>
  #include <list>
  #include <set>
  #include <unordered_set>
  #include "beyond_project.hpp"
#endif

//=============================================================================

int main()
{
  using namespace std;
  using namespace std::literals;

  {
    using uwindsor_2023w::comp3400::project::levenshtein;
    cout 
      << (levenshtein("kitten", "sitting") == 3) 
      << (levenshtein("Saturday", "Sunday") == 3)
      << (levenshtein("thou shalt not", "you should not") == 5)
      << (levenshtein("","") == 0)
      << (levenshtein(""s,""s) == 0)
      << (levenshtein(""s,vector<char>{}) == 0)
      // NOTE: A C-style string literal implicitly has and includes a '\0'
      //       character at the end of the string which is included in its 
      //       "length" as a literal value. This means the next line returns an
      //       edit distance of 1 instead of zero for an "empty" string.
      << (levenshtein(""s,"") == 1)
      << (levenshtein(string{"house"},"mouse"s) == 1)
      << (levenshtein(vector{'c','a','r'}, array{'b','a','t'}) == 2)
      << (levenshtein(wstring{L"αβδε"}, L"αβ_δε") == 2)
      << (levenshtein(vector{'V','s','a','u','c','e'}, "apple sauce"s) == 6)
      << '\n'
    ;
  }

#ifdef ALTERNATIVE_LEVENSHTEIN_IMPLEMENTATION
  {
    using uwindsor_2023w::comp3400::beyond_project::levenshtein;

    //
    // This code demonstrates using the general levenshtein() definition
    // in the sample solution project.hpp (than that required in the project). 
    // Notice this version can compute the Levenshtein edit distance with
    // two forward ranges.
    //
    auto const kitten = "kitten"s;
    auto const sitting = "sitting"s;

    forward_list<char> const kitten_fl{begin(kitten),end(kitten)};
    list<char> const kitten_l{begin(kitten),end(kitten)};
    list<char> const kitten_l_sorted = 
      [&]() { auto l = kitten_l; l.sort(); return l; }();
    unordered_set<char> const kitten_us{begin(kitten),end(kitten)};

    forward_list<char> const sitting_fl{begin(sitting),end(sitting)};
    list<char> const sitting_l{begin(sitting),end(sitting)};
    set<char> const sitting_se_sorted{begin(sitting),end(sitting)};
    unordered_set<char> const sitting_us{begin(sitting),end(sitting)};

    cout
      << (levenshtein(kitten, sitting) == 3)
      << (levenshtein(kitten_fl, sitting_l) == 3)
      << (levenshtein(kitten_fl, sitting_fl) == 3)
      // NOTE: std::set is not a sequence since it is sorted. Setting such aside
      //       such can still be used (assuming such use would be meaningful 
      //       with this function)...
      << (levenshtein(kitten_l_sorted, sitting_se_sorted) == 3)
      << (levenshtein(kitten_us, sitting_us) == 2)
      << '\n'
    ;
    // 
    // Other things that could be done:
    //   * If both arguments were sized_ranges, one could ensure the shortest 
    //     range is always the second argument to keep the std::vector objects 
    //     small (and therefore the amount of work done smaller).
    //
  }
#endif
}

//=============================================================================
