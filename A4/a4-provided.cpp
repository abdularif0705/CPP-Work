//=============================================================================

#include <array>
#include <istream>
#include <optional>
#include <string>
#include <sstream>

#include "a4-provided.hpp"

//=============================================================================

//
// std::optional<card_suit> read_card_suit(std::istream)
//
// read_card_suit reads a card suit from an input stream returning a
// an optional value. The return value only holds a card_suit if a valid 
// card_suit was read in. If an error occurs on the stream while reading in
// the data, then stream is failed. If the character on the stream read in
// is not a valid suit, then the character read in is returned to the stream
// using is.unget().
//
std::optional<card_suit> read_card_suit(std::istream& is)
{
  std::optional<card_suit> retval;
  auto ch = is.get();
  if (!std::istream::traits_type::eq_int_type(ch,std::istream::traits_type::eof()))
  {
    switch (ch)
    {
      case 'c':
        retval = card_suit::clubs;
        break;

      case 'd':
        retval = card_suit::diamonds;
        break;

      case 'h':
        retval = card_suit::hearts;
        break;

      case 's':
        retval = card_suit::spades;
        break;

      default:
        is.unget();                               // return ch to stream
        is.setstate(std::ios_base::failbit);      // fail the stream
    }
  }
  else
    is.setstate(std::ios_base::failbit);          // fail the stream
  return retval;
}

//=============================================================================

//
// Provide a simple way to generate all playing cards without requiring 
// card_face, card_suit, playing_card, and all supporting operations to be
// written first.
//
std::string all_playing_cards_as_string()
{
  std::array<char,4> const special_face{ 'A', 'J', 'Q', 'K' };
  std::array<char,4> const suit{ 'c', 'd', 'h', 's' };

  std::ostringstream buf;

  // generate all numeric face cards...
  for (int face=2; face != 11; ++face)
    for (auto const s : suit)
      buf << face << s;

  // generate all special face cards...
  for (auto const f : special_face)
    for (auto const s : suit)
      buf << f << s;

  // generate two jokers (red and white)...
  buf << "RW";

  // return string...
  return buf.str();
}

//=============================================================================
