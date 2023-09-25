//=============================================================================

#include <charconv>           // for std::from_chars
#include <compare>            // for std::strong_ordering
#include <concepts>           // for std::forward_iterator
#include <fstream>            // for std::ifstream
#include <functional>         // for std::function
#include <ios>                // for std::ios_base::failbit
#include <iostream>           // for std::cout, std::cerr
#include <map>                // for std::map
#include <optional>           // for std::optional
#include <sstream>            // for std::ostringstream
#include <stack>              // for std::stack
#include <stdexcept>          // for std::domain_error
#include <string>             // for std::string
#include <system_error>       // for std::errc

//=============================================================================

class mod5
{
private:
  unsigned i;

public:
  mod5() :
    i{}
  {
  }

  mod5(mod5 const&) = default;
  mod5& operator=(mod5 const&) = default;

  explicit mod5(unsigned value) :
    i{value}
  {
  }

  unsigned mod_value() const
  {
    return i % 5;
  }

  mod5& operator+=(mod5 const& rhs)
  {
    this->i += rhs.i;
    return *this;
  }

  mod5& operator-=(mod5 const& rhs)
  {
    this->i -= rhs.i;
    return *this;
  }

  mod5& operator*=(mod5 const& rhs)
  {
    this->i *= rhs.i;
    return *this;
  }

  mod5& operator/=(mod5 const& rhs)
  {
    if (rhs.mod_value() == 0) {
      std::ostringstream buf;
      buf << "mod5 divide by zero; operands(" << i << ',' << rhs.i << ')';
      throw std::domain_error(buf.str());
    }
    this->i /= rhs.mod_value();
    return *this;
  }

  friend bool operator==(mod5 const& lhs, mod5 const& rhs)
  {
    return lhs.mod_value() == rhs.mod_value();
  }

  friend auto operator<=>(mod5 const& lhs, mod5 const& rhs)
  {
    return lhs.mod_value() <=> rhs.mod_value();
  }
};

mod5 operator+(mod5 const& lhs, mod5 const& rhs)
{
  mod5 local_copy_lhs = lhs;
  local_copy_lhs += rhs;
  return local_copy_lhs;
}

mod5 operator-(mod5 const& lhs, mod5 const& rhs)
{
  mod5 local_copy_lhs = lhs;
  local_copy_lhs -= rhs;
  return local_copy_lhs;
}

mod5 operator*(mod5 const& lhs, mod5 const& rhs)
{
  mod5 local_copy_lhs = lhs;
  local_copy_lhs *= rhs;
  return local_copy_lhs;
}

mod5 operator/(mod5 const& lhs, mod5 const& rhs)
{
  mod5 local_copy_lhs = lhs;
  local_copy_lhs /= rhs;
  return local_copy_lhs;
}

std::istream &operator>>(std::istream &is, mod5 &m) {
  unsigned int temp;
  if (is >> temp) { m = mod5{temp}; }
  else { is.setstate(std::ios_base::failbit); }
  return is;
}

std::ostream& operator<<(std::ostream& os, mod5 const& m)
{
  os << m.mod_value();
  return os;
}

//=============================================================================

// std::optional<unsigned> either holds an unsigned value or it does not.
std::optional<unsigned> string_to_unsigned(std::string const& str)
{
  unsigned num;

  // [ptr,err] is an example of a "structured binding" --see ATOCPP textbook
  auto const [ptr, err] =
    std::from_chars(str.data(), str.data() + str.size(), num);

  if (err == std::errc() && (ptr == str.data() + str.size()))
    // no error and all data was consumed so construct return...
    return { num };
  else
    // otherwise default construct an empty optional value...
    return {};
}

//=============================================================================

// allow the s literal suffix to be used with string literals to yield
// strings of type std::string...
using namespace std::literals::string_literals;

// convenience type alias for code below...
using mod5_stack = std::stack<mod5>;

//=============================================================================

//
// std::map<Key,Value> is a dictionary ordered by key-base sorting Key.
//
// std::function<FnType> holds a callable entitiy of type FnType. In this
// case FnType is void(mod5_stack&), i.e., the function has void return and
// one parameter of type mod5_stack&.
//
std::map< std::string, std::function< void(mod5_stack&) > > const op_table{
  {
    "+"s,
    [](mod5_stack& stk)
    {
      if (stk.size() < 2)
        throw std::domain_error("insufficient number of stack entries");

      auto const op1 = stk.top();
      stk.pop();

      auto const op2 = stk.top();
      stk.pop();

      stk.push(op1 + op2);
    }
  },
  {
    "-"s,
    [](mod5_stack& stk)
    {
      if (stk.size() < 2)
        throw std::domain_error("insufficient number of stack entries");

      auto const op1 = stk.top();
      stk.pop();

      auto const op2 = stk.top();
      stk.pop();

      stk.push(op1 - op2);
    }
  },
  {
    "*"s,
    [](mod5_stack& stk)
    {
      if (stk.size() < 2)
        throw std::domain_error("insufficient number of stack entries");

      auto const op1 = stk.top();
      stk.pop();

      auto const op2 = stk.top();
      stk.pop();

      stk.push(op1 * op2);
    }
  },
  {
    "/"s,
    [](mod5_stack& stk)
    {
      if (stk.size() < 2)
        throw std::domain_error("insufficient number of stack entries");

      auto const op1 = stk.top();
      stk.pop();

      auto const op2 = stk.top();
      stk.pop();

      stk.push(op1 / op2);
    }
  }
};

//=============================================================================

//
// The <algorithm> header has many algorithms in it. This is an algorithm
// defined to apply op to each pair of values in the (half-open) range 
// [first,last).
//
template <std::forward_iterator It, typename Op>
void adjacent_apply(It first, It last, Op op)
{
  if (first == last)
    return;

  It prev = first;
  ++first;

  for (; first != last; ++prev, ++first)
    op(*prev,*first);
}

//-----

int simple_rel_demo(std::istream& in)
{
  std::vector<mod5> v;

  // process the input adding all unsigned values to v...
  for (std::string token; in >> token;)
  {
    auto const num = string_to_unsigned(token);
    if (num)
      v.push_back(mod5{*num});
  }

  // process v in outputting various results...
  std::cout << "rel_demo: operator==(): ";
  adjacent_apply(
    v.begin(), v.end(),
    [](auto const& a, auto const& b)
    {
      std::cout << (a == b);
    }
  );
  std::cout << '\n';

  std::cout << "rel_demo: operator<(): ";
  adjacent_apply(
    v.begin(), v.end(),
    [](auto const& a, auto const& b)
    {
      std::cout << (a < b);
    }
  );
  std::cout << '\n';

  std::cout << "rel_demo: operator>(): ";
  adjacent_apply(
    v.begin(), v.end(),
    [](auto const& a, auto const& b)
    {
      std::cout << (a > b);
    }
  );
  std::cout << '\n';
  return 0;
}

inline int simple_rel_demo(std::istream&& in)
{
  // in is an lvalue that refers to an rvalue
  // the next call invokes simple_rpn_demo(std::istream&)...
  return simple_rel_demo(in);
}

//=============================================================================

int simple_rpn_demo(std::istream& in)
{
  //
  // declare stack and process input...
  //
  mod5_stack stk;
  for (std::string token; in >> token;)
  {
    // if any exceptions occur, output such to cerr and continue...
    try
    {
      auto const num = string_to_unsigned(token);
      if (num)
      {
        // construct a mod5 object from *num and push it onto stk...
        stk.push(mod5{*num});
        continue;
      }
      else
      {
        // search op_table for token
        auto pos = op_table.find(token);
        if (pos != op_table.end())
          // found token, call function passing in stk...
          pos->second(stk);
        else
          // token not found, output error...
          std::cerr << "ERROR: Unknown operation: " << token << '\n';
      }
    }
    catch (std::domain_error& ex)
    {
      std::cerr << "EXCEPTION: " << ex.what() << '\n';
    }
    catch (...)
    {
      std::cerr << "FATAL: Unknown exception. Aborting.\n";
      return 1;
    }
  }

  //
  // dump stack...
  //
  std::cout << "stack(" << stk.size() << "):";
  while (!stk.empty())
  {
    std::cout << ' ' << stk.top();
    stk.pop();
  }
  std::cout << '\n';
  return 0;
}

inline int simple_rpn_demo(std::istream&& in)
{
  // in is an lvalue that refers to an rvalue
  // the next call invokes simple_rpn_demo(std::istream&)...
  return simple_rpn_demo(in);
}

//=============================================================================

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }

  // call simple_rel_demo passing in a std::istream file object...
  simple_rel_demo(std::ifstream(argv[1]));

  // call simple_rpn_demo passing in a std::istream file object...
  return simple_rpn_demo(std::ifstream(argv[1]));
}

//=============================================================================
