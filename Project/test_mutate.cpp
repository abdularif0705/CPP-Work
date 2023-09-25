//=============================================================================

#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include "project.hpp"

//=============================================================================

int main()
{
  using uwindsor_2023w::comp3400::project::mutate;
  using uwindsor_2023w::comp3400::project::char_mutator;

  std::string const str{ "To be or not to be." };
  std::cout << std::quoted(str) << '\n';

  char_mutator m;
  std::default_random_engine re{std::random_device{}()};
  for (int i{}; i != 10; ++i)
  {
    auto str2 = str;
    mutate(str2, 0.25, m, re);
    std::cout << std::quoted(str2) << '\n';
  }
}

//=============================================================================
