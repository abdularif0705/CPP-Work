#ifndef uwindsor_2023w_comp3400_project_utils_hpp_
#define uwindsor_2023w_comp3400_project_utils_hpp_

//=============================================================================

#include <concepts>
#include <iterator>
#include <type_traits>

//=============================================================================

namespace uwindsor_2023w {
namespace comp3400 {
namespace project {

//=============================================================================

template <typename T, typename Int>
concept reservable =
  requires (T t, Int n)
  {
    { t.reserve(n) };
  }
;

template <typename Container, typename Int>
constexpr void reserve_or_noop(Container& c, Int const& n)
{
  if constexpr(reservable<Container,Int>)
    c.reserve(n);
}

//=============================================================================

template <typename Container>
concept back_insertable =
  std::ranges::range<Container> &&
  requires (Container c)
  {
    { std::back_inserter(c) } -> 
      std::output_iterator<std::ranges::range_value_t<Container>>;
    // NOTE: The std::output_iterator<A,B> concept, to be true, needs the A 
    //       parameter be an output iterator type and the B parameter to be 
    //       a suitable value type. Using std::output_iteator<T> here
    //       works because the compiler takes the type of what is returned 
    //       from the expression inside { } and passes it as the first
    //       parameter of std::output_iterator.
  }
;

template <typename Container>
concept front_insertable =
  std::ranges::range<Container> &&
  requires (Container c)
  {
    { std::front_inserter(c) } -> 
      std::output_iterator<std::ranges::range_value_t<Container>>;
  }
;

template <typename Container>
concept insertable =
  std::ranges::range<Container> &&
  requires (Container c)
  {
    { std::inserter(c) } -> 
      std::output_iterator<std::ranges::range_value_t<Container>>;
  }
;

//=============================================================================

//
// smart_insertable<Container>
// concept
//
// This concept is true if Container is back_insertable, front_insertable, or
// insertable.
//
template <typename Container>
concept smart_insertable = 
  back_insertable<Container> || 
  front_insertable<Container> ||
  insertable<Container>
;

//
// smart_inserter(c)
// function template
//
// This function returns an output iterator that will insert items in to c.
// Back insertion is preferred over front insertion which are preferred over
// general insertion (at the end / hinting at the end).
//
template <smart_insertable Container>
constexpr auto smart_inserter(Container& c)
{
  if constexpr(back_insertable<Container>)
    return std::back_inserter(c);
  else if constexpr(front_insertable<Container>)
    return std::front_inserter(c);
  else
    return std::inserter(c, end(c));
}

//=============================================================================

} // namespace project
} // namespace comp3400
} // namespace uwindsor_2023w

//=============================================================================

#endif // #ifndef uwindsor_2023w_comp3400_project_utils_hpp_
