#ifndef a5_provided_hpp_
#define a5_provided_hpp_

//=============================================================================

#include <istream>
#include <iomanip>              // e.g., for std::quoted
#include <ostream>
#include <string>

//=============================================================================

class playing_card_company
{
private:
  std::string name_;

public:
  playing_card_company() = default;
  playing_card_company(playing_card_company const&) = default;
  playing_card_company(playing_card_company&&) = default;
  playing_card_company& operator=(playing_card_company const&) = default;
  playing_card_company& operator=(playing_card_company&&) = default;

  playing_card_company(std::string name) :
    name_{std::move(name)}
  {
  }

  playing_card_company& operator=(std::string name)
  {
    name_ = std::move(name);
    return *this;
  }

  explicit operator bool() const { return !name_.empty(); }
  bool is_valid() const { return !name_.empty(); }

  std::string const& name() const { return name_; }
};

//=============================================================================

inline std::istream& operator>>(std::istream& is, playing_card_company& c)
{
  std::string name;
  if (is >> std::quoted(name))
    c = std::move(name);
  return is;
}

inline std::ostream& operator<<(std::ostream& os, playing_card_company const& c)
{
  os << std::quoted(c.name());
  return os;
}

//=============================================================================

#endif // #ifndef a5_provided_hpp_
