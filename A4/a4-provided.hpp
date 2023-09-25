#ifndef a4_provided_hpp_
#define a4_provided_hpp_

//=============================================================================

#include <compare>          // e.g., for operator<=>
#include <istream>          // e.g., for std::istream
#include <optional>         // e.g., for std::optional
#include <ostream>          // e.g., for std::ostream
#include <string>           // e.g., for std::string
#include <type_traits>      // e.g., for std::underlying_type_t

//=============================================================================

enum class card_suit { clubs, spades, hearts, diamonds };
std::optional<card_suit> read_card_suit(std::istream& is);

inline std::istream& operator>>(std::istream& is, card_suit& s)
{
  auto result = read_card_suit(is);
  if (result)
    s = *result;
  return is;
}

inline std::ostream& operator<<(std::ostream& os, card_suit const& s)
{
  switch (s)
  {
    case card_suit::clubs:      os.put('c'); break;
    case card_suit::diamonds:   os.put('d'); break;
    case card_suit::hearts:     os.put('h'); break;
    case card_suit::spades:     os.put('s'); break;

    default: 
      os.setstate(std::ios_base::failbit); 
      break;
  }
  return os;
}

//=============================================================================

enum class card_face
{
  ace, two, three, four, five,
  six, seven, eight, nine, ten,
  jack, queen, knight, king,
  red_joker,
  white_joker
};

// NOTE: You must define these functions in your code:
std::optional<card_face> read_card_face(std::istream& is);
std::istream& operator>>(std::istream& is, card_face& s);
std::ostream& operator<<(std::ostream& os, card_face const& s);

//=============================================================================

// NOTE: You must define this class in your code:
class playing_card;

// NOTE: You must define these functions in your code:
bool operator==(playing_card const&, playing_card const&);
std::strong_ordering operator<=>(playing_card const&, playing_card const&);
std::optional<playing_card> read_playing_card(std::istream& is);
std::istream& operator>>(std::istream& is, playing_card& c);
std::ostream& operator<<(std::ostream& os, playing_card const& c);

//=============================================================================

std::string all_playing_cards_as_string();

//=============================================================================

#endif // #ifndef a4_provided_hpp_
