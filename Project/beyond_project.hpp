#ifndef uwindsor_2023w_comp3400_beyond_project_hpp_
#define uwindsor_2023w_comp3400_beyond_project_hpp_

//=============================================================================

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <vector>
#include <iostream>

#include "utils.hpp"
#include "project.hpp"

//=============================================================================

namespace uwindsor_2023w {
namespace comp3400 {
namespace beyond_project {

//=============================================================================

//
// The min symbols in this namespace are to use the ones in the 
// ::uwindsor_2023w::comp3400::project namespace.
//
using ::uwindsor_2023w::comp3400::project::min;

//=============================================================================

//
// num_args(args...)
// Returns the number of args...
//
template <typename... Args>
inline constexpr auto num_args(Args&&...) noexcept
{
  return sizeof...(Args);
}

// some quick static_assert tests...
static_assert(num_args() == 0);
static_assert(num_args(0) == 1);
static_assert(num_args(0,"") == 2);
static_assert(num_args(0,"",'a') == 3);

//=============================================================================

//
// nth_arg<Index>(args...)
// Returns the nth argument perfectly forwarded. If there are no arguments
// or if the Index doesn't map to an argument then the return type is void.
//
template <std::size_t Index, typename Arg, typename... Args>
inline constexpr decltype(auto) nth_arg(Arg&& arg, Args&&... args) noexcept
{
  if constexpr(sizeof...(Args)+1 <= Index)
    return; // i.e., return void
  if constexpr(Index == 0)
    return std::forward<Arg>(arg);
  else
    return nth_arg<Index-1>(std::forward<Args>(args)...);
}

// some quick static_assert tests...
static_assert(nth_arg<0>(99,3.14,'a') == 99);
static_assert(nth_arg<1>(99,3.14,'a') == 3.14);
static_assert(nth_arg<2>(99,3.14,'a') == 'a');

//=============================================================================

//
// Sometimes namespaces are used for implementation-specific definitions that
// users shouldn't use. A common namespace name used in libraries to hold
// definitions users should never use is called "detail". Here the detail
// namespace serves to provide a definition of nth_type_impl.
//
namespace detail {

template <std::size_t Index, typename T, typename... TS>
struct nth_type_impl
{
  using type = typename nth_type_impl<Index-1,TS...>::type;
};

// The next nth_type_impl is a partial specialization of the general form.
// Notice the template arguments are partially fixed.
template <typename T, typename... TS>
struct nth_type_impl<0,T,TS...>
{
  using type = T;
};

} // namespace detail

template <std::size_t Index, typename T, typename... TS>
requires (Index < sizeof...(TS)+1)
using nth_type_t = typename detail::nth_type_impl<Index,T,TS...>::type;

template <typename... TS>
inline constexpr auto num_types_v = sizeof...(TS);

// some quick static_assert tests...
static_assert(std::is_same_v<nth_type_t<0,int,char,double>,int>);
static_assert(std::is_same_v<nth_type_t<1,int,char,double>,char>);
static_assert(std::is_same_v<nth_type_t<2,int,char,double>,double>);
static_assert(num_types_v<int,char,double> == 3);

//=============================================================================

//
// Namespaces can be closed and reopened later to add more definitions.
// Template parameters can have three things in them:
//
//   1) a type, e.g., using typename or class
//   2) a (compile-time constant) value, e.g., std::size_t or auto
//   3) a "template template parameter"
//
// nth_value_impl holds all values.
//
namespace detail {

template <std::size_t Index, auto Value, auto... Values>
struct nth_value_impl
{
  static constexpr auto value = nth_value_impl<Index-1,Values...>::value;
};

// The next nth_value_impl is a partial specialization of the general form.
// Notice the template arguments are partially fixed.
template <auto Value, auto... Values>
struct nth_value_impl<0,Value,Values...>
{
  static constexpr auto value = Value;
};

} // namespace detail

template <std::size_t Index, auto Value, auto... Values>
requires (Index < sizeof...(Values)+1)
inline constexpr auto nth_value_v = 
  detail::nth_value_impl<Index,Value,Values...>::value
;

template <auto... Values>
inline constexpr auto num_values_v = sizeof...(Values);

// some quick static_assert tests...
static_assert(nth_value_v<0,10,'a'> == 10);
static_assert(nth_value_v<1,10,'a'> == 'a');
static_assert(num_values_v<10,'a'> == 2);

//=============================================================================

//
// range_size(r)
// Computes the size of range r in O(n) time. If r is a sized_range than
// it computes the size of the range in O(1) time.
//
// Precondition: r is not an infinite range.
// Postcondition: Return value is equivalent to >= 0.
//
template <std::ranges::range R>
inline constexpr auto range_size(R&& r)
{
  namespace rng = std::ranges;
  if constexpr(rng::sized_range<R>)
    return rng::size(r); // O(1) time
  else if constexpr(rng::common_range<R>)
    return distance(rng::cbegin(r), rng::cend(r)); // O(n) time
  else // O(n) time, n == size of r
  {
    // Manually determine the size of this range...
    size_t sz{};
    for (auto i = rng::cbegin(r); i != rng::cend(r); ++i)
      ++sz;
    return sz;
  }
}

//=============================================================================

template <std::ranges::range... RS>
requires (sizeof...(RS) > 0)
inline constexpr auto min_range_size(RS&&... rs)
{
  using namespace std;
  namespace rng = std::ranges;
 
  if constexpr((rng::sized_range<RS> && ...))
  {
    // If all ranges happen to be sized_ranges, use more efficient code...
    using uint_type = std::common_type_t<decltype(std::ranges::size(rs))...>;
    return min(uint_type{std::ranges::size(std::forward<RS>(rs))}...);
  }
  else
  {
    // Otherwise traverse every range until the first end is found...

    // Obtain a tuple of all ranges' begin() iterator...
    tuple<rng::iterator_t<RS>...> iters{ rng::cbegin(rs)... };

    // Obtain a tuple of all ranges' end() iterator/sentinel...
    tuple<rng::sentinel_t<RS>...> const end_iters{ rng::cend(rs)... };

    // lambda template whose argument is used to pass in iter's index
    // values (at compile-time!) so comparisons can be done with 
    // the corresponding iters and end_iters values...
    auto is_not_done =
      [&]<std::size_t... Indices>(std::index_sequence<Indices...>) constexpr
      {
        // Apply != to std::get() returned results using a C++17 fold
        // expression...
        return ((get<Indices>(iters) != get<Indices>(end_iters)) && ...);
      }
    ;

    // lambda template whose argument is used to pass in iter's index
    // values (at compile-time!) so each iter value can be incremented...
    auto incr =
      [&]<std::size_t... Indices>(std::index_sequence<Indices...>) constexpr
      {
        // Apply ++ to std::get using a C++17 fold expression...
        return (++get<Indices>(iters), ...);
      }
    ;

    // Iterate all iterators until the first arrives at its end, tracking the
    // number of iterations have occurred. The result is the minimum ranges' 
    // size...
    size_t sz{};
    for (
      ; 
      is_not_done(std::make_index_sequence<sizeof...(RS)>{}); 
      incr(std::make_index_sequence<sizeof...(RS)>{})
    )
      ++sz;
    return sz;
  }
}

//=============================================================================

// https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
template <typename StringA, typename StringB>
requires
  std::ranges::forward_range<StringA> &&
  std::ranges::forward_range<StringB> &&
  std::same_as<
    std::ranges::range_value_t<StringA>,
    std::ranges::range_value_t<StringB>
  >
constexpr std::size_t levenshtein(StringA const& a, StringB const& b)
{
  using namespace std;

  //
  // This example solution is more general than project requirements and is not
  // what one was to write in the project. It has been provided so you can see
  // that:
  //   * one is able to relax the requirement that StringA and StringB must
  //     be std::ranges::forward_range which allows this function to be used
  //     with more range types, and,
  //       - Notice the code using iterators instead of indices which works
  //         since things are nicely processed in passed and the values needed
  //         to be accessed and stored can easily be computed.
  //   * one is able to remove the sized_range requirements at the cost of
  //     some time --but such is one (quick) single-pass compared to the 
  //     multiple-passes required in the Levenshtein dynamic programming 
  //     algorithm code.
  //

  auto const bsize = range_size(b);

  vector<size_t> prev_row(bsize+1);
  iota(prev_row.begin(), prev_row.end(), size_t{});

  vector<size_t> cur_row(bsize+1, 0);

  size_t i_dist_plus_one{1};
  for (auto i=ranges::cbegin(a); i != ranges::cend(a); ++i, ++i_dist_plus_one)
  {
    cur_row.front() = i_dist_plus_one;
    {
      auto cur_row_iter = cur_row.begin();
      auto prev_row_iter = prev_row.begin();
      for (
        auto j=ranges::cbegin(b); 
        j != ranges::cend(b); 
        ++j, ++prev_row_iter, ++cur_row_iter
      )
      {
        size_t const insert_cost = *cur_row_iter+1;
        size_t const subst_cost = *prev_row_iter + (*i != *j);
        size_t const del_cost = *std::next(prev_row_iter)+1;
        *std::next(cur_row_iter) = min(del_cost, insert_cost, subst_cost);
      }
    }
    swap(prev_row, cur_row);
  }
  return prev_row.back();
}

//=============================================================================

//using ::uwindsor_2023w::comp3400::project::char_mutator;
//using ::uwindsor_2023w::comp3400::project::mutate;

//=============================================================================

//
// region_sample_iterator<URBG,UInt>
// class template
//
// region_sample_iterator is a forward iterator that performs effectively the 
// same task as std::sample() except that it selects k "subranges" from the 
// integer sequence [0,n) such that each range has an equal probability of 
// appearance.
//
// This implies that there will be at most k regions will be determined
// pseudorandomly, i.e., it is possible for fewer regions to be determined.
//
// This iterator has been defined for two reasons:
//
//   * to avoid having to formally store std::sample() results in a container,
//     and,
//
//   * to properly handle regions instead of samples.
//       - k samples taken between elements in [0,n) will result in k+1 regions
//       - e.g., the last region is typically "short" and this iterator ensures
//         the last interval extends to n.
//
// This iterator's value_type is a struct representing the current subrange
// as:
//
//   struct
//   {
//     UInt reverse_id;
//     UInt id;
//     UInt first;
//     UInt last;
//   };
//
// where such represents:
//
//   * id is the 0-based nth sampled region id
//   * reverse_id is the 0-based nth sampled region id from the last interval
//   * the half-open range [first,last)
//
// These values easily allow one to determine each subrange's size, etc.
//
// NOTE: The constructor of region_sample_iterator accepts a reference
//       to a URBG (uniform_random_bit_generator). The URBG lifetime
//       must be at least as long as iterators' lifetimes using them.
//
template <typename URBG, typename UInt = std::size_t>
requires std::uniform_random_bit_generator<std::remove_cvref_t<URBG>>
class region_sample_iterator
{
public:
  using urbg_type = std::remove_cvref_t<URBG>;
  using uint_type = UInt;

private:
  struct region
  {
    uint_type reverse_id;
    uint_type id;
    uint_type from;
    uint_type to;

    constexpr bool operator==(region const&) const noexcept = default;
  };

public:
  using value_type = region const;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::forward_iterator_tag;

private:
  // std::optional is used to hold std::reference_wrapper<URBG> to allow the
  // default constructor to be an end iterator (i.e., no reference is held to
  // any URBG in the end iterator)...
  std::optional<std::reference_wrapper<urbg_type>> urbg_;
  uint_type const pop_size_{};
  uint_type num_in_pop_left_;
  uint_type last_i_;
  uint_type i_;
  region region_;

  constexpr void next()
  {
    if (region_.reverse_id == 0)
      // Done. Make this iterator an end iterator...
      urbg_.reset();
    else
    {
      for (bool not_found = true; not_found; ++i_)
      {
        uint_type const r = std::uniform_int_distribution<uint_type>
          (0,--num_in_pop_left_)(urbg_->get());
        ;

        if (r < region_.reverse_id)
        {
          // Region [last_i_,i_) is the next region...
          if (last_i_ > 0)
            ++region_.id;
          region_.from = last_i_;
          // If --region.reverse_id == 0 then this is the last interval so 
          // region_.to needs to be set to pop_size_...
          region_.to = (--region_.reverse_id > 0) ? i_ : pop_size_;

          last_i_ = i_;
          not_found = false;
        }
      }
    }
  }

public:
  constexpr region_sample_iterator() = default; // i.e., the end iterator
  constexpr region_sample_iterator(region_sample_iterator const&) = default;
  constexpr region_sample_iterator(region_sample_iterator&&) = default;
  constexpr region_sample_iterator& operator=(region_sample_iterator const&) = default;
  constexpr region_sample_iterator& operator=(region_sample_iterator&&) = default;

  constexpr region_sample_iterator(
    URBG& urbg,
    uint_type const& pop_size, 
    uint_type const& num_regions
  ) :
    urbg_{urbg},
    pop_size_{pop_size},
    num_in_pop_left_{pop_size},
    last_i_{},
    i_{1},      // i_ needs to be one ahead of last_i_ to start
    region_{
      min(num_regions,pop_size),  // reverse id
      {},                         // starting id
      {}, {}                      // [from,to)
    }
  {
    // NOTES: Reservoir sampling will generate k PRNG values thus:
    //   * num_regions will correspond to k-1 samples
    //   * pop_size can only have at most pop_size-1 regions as each
    //     region must at least be of size one

    if (num_regions == 0)
    {
      // no region, set last interval to be empty
      region_.reverse_id = 0;
      region_.id = 0;
      region_.from = region_.to = 0;
    }
    else if (num_regions <= 1)
    {
      // nothing to do, set last interval
      region_.reverse_id = 0;
      region_.id = 0;
      region_.from = 0;
      region_.to = pop_size;
    }
    else
      // compute the first region...
      next();
  }
  
  constexpr bool operator==(region_sample_iterator const& b) const
  {
    bool const region_match = (region_ == b.region_);
    bool const urbg_same = (urbg_ && b.urbg_ && *urbg_ == *b.urbg_);
    bool const urbg_both_are_end = (!urbg_ == !b.urbg_);
    return urbg_both_are_end || (urbg_same && region_match);
  }

  constexpr bool operator!=(region_sample_iterator const& b) const
  {
    return !operator==(b);
  }

  constexpr reference operator*() const
  {
    return region_;
  }
  
  constexpr pointer operator->() const
  {
    return &region_;
  }

  constexpr region_sample_iterator& operator++()
  {
    next();
    return *this;
  }

  constexpr region_sample_iterator operator++(int)
  {
    region_sample_iterator tmp{*this};
    this->operator++();
    return tmp;
  }
};

//=============================================================================

//
// This example solution is more general than project requirements and is not
// what one was to write in the project. It has been provided so you can see
// other ways to continue to evolve and improve the code for various
// design needs/desires.
//
// "Problems" with the required crossover implementation include:
//   * it unnecessarily dynamically allocates RAM for two vectors
//       - To address this is to directly implement a form of std::sample()
//         (reservoir sampling) but do not store the indices. Instead copy
//         the current "chunk" to copy from the appropriate parent to the
//         output. This avoids storing any indices and therefore avoids
//         using extra memory proportional to the number of crossover points.
//   * it (likely) requires dynamically allocating RAM to generate
//     the return crossover individual
//       - A suitable way to address this is to have the caller pass in an
//         output iterator --allowing the caller to decide how the crossover
//         result is stored. For example, if the output iterator is writing
//         to a sufficiently long fixed-length, already allocated container, 
//         then no extra RAM is needed at all.
//       - Such also implies that the return value should be the output 
//         iterator.
//   * one might not always have sized ranges
//       - This is handled by calling min_range_size() defined earlier in this
//         file.
//   * there is no need to require both parents to be of the same type
//       - e.g., each parent range is read-only and only copied from.
//       - Each parent's range's elements must be the same type. This can be
//         enforced using a requires clause.
//

template <
  typename URBG1,
  typename URBG2,
  std::ranges::forward_range Parent1,
  std::ranges::forward_range Parent2,
  typename OutIndividual
>
requires
  // URBG1 and URBG2 must be std::uniform_random_bit_generator concepts...
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG1>> &&
  std::uniform_random_bit_generator<std::remove_cvref_t<URBG2>> &&
  // Require both parents to have the same underlying element type...
  std::same_as<
    std::ranges::range_value_t<Parent1>,
    std::ranges::range_value_t<Parent2>
  > &&
  // Require the output to have the same underlying element type as parents...
  std::output_iterator<OutIndividual, std::ranges::range_value_t<Parent1>>
constexpr auto crossover(
  std::size_t const ncrossover_points,
  URBG1&& urbg_starting_parent,
  URBG2&& urbg_crossover_points,
  Parent1 const& parent1,
  Parent2 const& parent2,
  OutIndividual out,
  bool const copy_longer_range_tail = true
) -> OutIndividual
{
  using namespace std;
  size_t const sz = min_range_size(parent1, parent2);

  auto p1it = ranges::cbegin(parent1);

  auto p2it = ranges::cbegin(parent2);

  bernoulli_distribution bd(0.5);
  bool which_parent = bd(urbg_starting_parent); // false is p1, true is p2

  region_sample_iterator i{urbg_crossover_points, sz, ncrossover_points+1};
  region_sample_iterator<std::remove_cvref_t<URBG2>> i_end{};

  for (; i != i_end; ++i)
  {
    auto const sz = i->to - i->from;

    if (which_parent)
      out = ranges::copy_n(p1it, sz, out).out;
    else
      out = ranges::copy_n(p2it, sz, out).out;

    which_parent = !which_parent;
    advance(p1it, sz);
    advance(p2it, sz);
  }

  if (copy_longer_range_tail)
  {
    if (!which_parent)
      out = ranges::copy(p1it, ranges::cend(parent1), out).out;
    else
      out = ranges::copy(p2it, ranges::cend(parent2), out).out;
  }
  return out;
}

//=============================================================================

} // namespace beyond_project
} // namespace comp3400
} // namespace uwindsor_2023w

//=============================================================================

#endif // #ifndef uwindsor_2023w_comp3400_beyond_project_hpp_
