#ifndef uwindsor_2023w_comp3400_project_hpp_
#define uwindsor_2023w_comp3400_project_hpp_

//=============================================================================

#include <algorithm>
#include <array>
#include <cctype>
#include <concepts>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "utils.hpp"

//=============================================================================

namespace uwindsor_2023w {
namespace comp3400 {
namespace project {

//=============================================================================

constexpr void min() = delete;

template <typename T>
constexpr T const& min(T const& a)
{
  return a;
}

template <typename T>
constexpr T const& min(T const& a, T const& b)
{
  return (b < a) ? b : a;
}

template <typename T, typename... Rest>
requires (std::same_as<T,Rest> && ...)
constexpr T const& min(T const& a, T const& b, Rest const&... rest)
{
#if 0
  if (b < a)
    return min(b, rest...);
  else
    return min(a, rest...);
#else
  return min(min(a,b),rest...);
#endif
}

//=============================================================================

// https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
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
std::size_t levenshtein(StringA const& a, StringB const& b)
{
  using namespace std;

  auto const bsize = ranges::size(b);

  vector<size_t> prev_row(bsize+1);
  iota(prev_row.begin(), prev_row.end(), size_t{});

  vector<size_t> cur_row(bsize+1, 0);

  auto const asize = ranges::size(a);
  auto a_iter = ranges::cbegin(a);    // or ranges::begin()
  auto b_iter = ranges::cbegin(b);    // or ranges::end()
  for (size_t i{}; i != asize; ++i)
  {
    cur_row.front() = i+1;
    for (size_t j{}; j != bsize; ++j)
    {
      size_t const insert_cost = cur_row[j]+1;
      size_t const subst_cost = prev_row[j] + (a_iter[i] != b_iter[j]);
      size_t const del_cost = prev_row[j+1]+1;
      cur_row[j+1] = min(del_cost, insert_cost, subst_cost);
    }
    swap(prev_row, cur_row);
  }
  return prev_row.back();
}

//=============================================================================

class char_mutator
{
private:
  std::string valid_chars;
  mutable std::uniform_int_distribution<std::size_t> ud;
  mutable std::default_random_engine re;

public:
  char_mutator() :
    valid_chars{
      []()
      {
        std::string retval;
        for (short i{}; i != std::numeric_limits<char>::max()+1; ++i)
          if (std::isalnum(i) || std::ispunct(i) || (i == ' '))
            retval.push_back(i);
        return retval;
      }()
    },
    ud(0, valid_chars.size() != 0 ? valid_chars.size()-1 : 0),
    re(std::random_device{}())
  {
  }

  template <typename... Args>
  char operator()(Args&&...) const
  {
    return valid_chars[ud(re)];
  }
};

//=============================================================================

// Mutate elements in the range individual, randomly and uniformly using rate
// and urbg. URBG is per the same in std::sample's parameter for randomness.
template <
  std::ranges::range Individual,
  typename MutateOp,
  typename URBG
>
requires 
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG>> &&
  std::invocable<MutateOp,std::ranges::range_value_t<Individual>>
void mutate(
  Individual& individual, 
  double const rate, 
  MutateOp&& m,
  URBG&& urbg
)
{
  using namespace std;

  if (individual.empty())
    return;

  std::uniform_real_distribution<double> ud(0.0,1.0);
  ranges::for_each(
    individual,
    [&](auto& element)
    {
      if (ud(urbg) < rate)
        element = m(element);
    }
  );
}

//=============================================================================

// https://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)
//   * URBG is per the same in std::sample's parameter for randomness.
//   * NOTE: Individual's are ranges --not binary arrays.
//   * NOTE: This code is a k-point cross over algorithm where
//             the number of crossover points is determined randomly (and
//             uniformly from 0 to minimum lengths of parent1 and parent2),
//             and, the actual points of crossover are then determined
//             uniformly over such using std::ranges::sample.
template <typename URBG1, typename URBG2, typename Individual>
requires
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG1>> &&
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG2>> &&
  std::ranges::forward_range<Individual> &&
  std::ranges::sized_range<Individual> &&
  smart_insertable<Individual>
auto crossover(
  std::size_t const ncrossover_points,
  URBG1&& urbg_starting_parent,
  URBG2&& urbg_crossover_points,
  Individual const& parent1,
  Individual const& parent2
) -> std::remove_cvref_t<Individual>
{
  using namespace std;
  using retval_type = remove_cvref_t<Individual>;

  size_t const psize_truncated = min(ranges::size(parent1), ranges::size(parent2));

  // Declare which_parent to copy from flag...
  //   * which_parent is the starting parent to use
  //   * false is parent1, true is parent2
  //   * determine such randomly
  bernoulli_distribution bd(0.5);
  bool which_parent = bd(urbg_starting_parent);

  // Are there any crossover points?
  if (psize_truncated == 0 || ncrossover_points == 0)
    // There are no crossover points. Return one of the parents as the result.
    return which_parent ? parent1 : parent2;

  // NOTE: There is at least 1 crossover point and both ranges are !empty()

  // Generate indices [1,psize_truncated-1)...
  //   * NOTE: Remember that crossover points occur *between* range elements.
  vector<size_t> crossover_indices(psize_truncated-1);
  iota(crossover_indices.begin(), crossover_indices.end(), size_t{1});

  // Select ncrossover_points...
  vector<size_t> crossover_offsets;
  reserve_or_noop(crossover_offsets, ncrossover_points);
  ranges::sample(
    crossover_indices, 
    back_inserter(crossover_offsets), 
    ncrossover_points, 
    urbg_crossover_points
  );

  // Determine adjacent differences so indices are offsets instead...
  adjacent_difference(
    begin(crossover_offsets), end(crossover_offsets),
    begin(crossover_offsets)
  );

  // Declare an individual and an output insert iterator to such...
  retval_type retval;
  reserve_or_noop(
    retval, 
    std::max(ranges::size(parent1), ranges::size(parent2))
  );
  auto out = smart_inserter(retval);

  // Copy parent subranges to to-be-returned Individual...
  // Set initial iterator positions...
  auto p1pos = ranges::cbegin(parent1);
  auto p2pos = ranges::cbegin(parent2);

  // Now perform the crossover copying...
  for (auto const& offset : crossover_offsets)
  {
    if (which_parent)
      out = copy_n(p1pos, offset, out);
    else
      out = copy_n(p2pos, offset, out);

    // advance p1pos and p2pos...
    advance(p1pos, offset);
    advance(p2pos, offset);
    which_parent = !which_parent;
  }

  // copy last chunk...
  if (which_parent)
    ranges::copy(p1pos, ranges::cend(parent1), out);
  else
    ranges::copy(p2pos, ranges::cend(parent2), out);
  return retval;
}

//=============================================================================

} // namespace project
} // namespace comp3400
} // namespace uwindsor_2023w

//=============================================================================

#endif // #ifndef uwindsor_2023w_comp3400_project_hpp_
