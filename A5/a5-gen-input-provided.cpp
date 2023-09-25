//=============================================================================

#include <cassert>
#include <algorithm>
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>

#include "a4-provided.hpp"
#include "a5-provided.hpp"
#include "a5-random-provided.hpp"
#include "a4-include.hpp"

//=============================================================================

std::array const playing_card_companies{
  "Bicycle Cards",
  "KEM",
  "Hoyle",
  "Bee Playing Cards",
  "Tally-Ho Playing Cards",
  "Aviator",
  "Copag",
  "Modiano"
};
  
constexpr std::array suits{
  card_suit::clubs,
  card_suit::spades,
  card_suit::hearts,
  card_suit::diamonds
};

constexpr std::array faces_with_suits{
  card_face::ace, card_face::two, card_face::three, card_face::four,
  card_face::five, card_face::six, card_face::seven, card_face::eight,
  card_face::nine, card_face::ten, card_face::jack, card_face::knight,
  card_face::queen, card_face::king
};

constexpr std::array faces_without_suits{
  card_face::red_joker,
  card_face::white_joker
};

//=============================================================================

struct pcc
{
  playing_card card;
  playing_card_company comp;
};

std::ostream& operator<<(std::ostream& os, pcc const& c)
{
  os << c.card << c.comp;
  return os;
}

//=============================================================================

auto generate_full_deck()
{
  using namespace std;

  vector<playing_card> retval;
  retval.reserve(58);

  for (auto const& f : faces_with_suits)
    for (auto const& s : suits)
      retval.push_back({ f, s });

  for (auto const& f : faces_without_suits)
    retval.push_back({ f });

  return retval;
}

//=============================================================================

auto generate_company_full_deck(playing_card_company const& c)
{
  using namespace std;

  auto deck = generate_full_deck();

  vector<pcc> retval;
  retval.reserve(deck.size());
  transform(
    begin(deck), end(deck),
    back_inserter(retval),
    [&c](playing_card const& card)
    {
      return pcc{card,c};
    }
  );

  return retval;
}

//=============================================================================

auto generate_input(
  std::size_t max_decks_from_company,
  double missing_prob = 1.0/80.0
)
{
  using namespace std;

  uniform_int_distribution<size_t> ud(1,max_decks_from_company);

  auto re = make_random_mersenne_engine();
  vector<pcc> retval;

  // for each card company...
  for (auto const& company : playing_card_companies)
  {
    // for a ud number of decks from company...
    size_t const ndecks = ud(re);
    clog << "DEBUG: Company " << quoted(company) << " will have " 
      << ndecks << " decks\n";
    for (size_t cnt = ndecks; cnt != 0; --cnt)
    {
      // start with a full deck...
      auto deck = generate_company_full_deck(playing_card_company{company});

      // and append such to retval...
      copy_n(begin(deck), deck.size(), back_inserter(retval));
    }
  }
     
  // ramdomize all cards...
  auto re_shuffle = make_random_mersenne_engine();
  shuffle(begin(retval), end(retval), re_shuffle);

  // and remove some cards...
  //   * bd uses the next point obtained via ed(re_ed) as the probability
  //     of deletion
  auto re_bd = make_random_mersenne_engine();
  bernoulli_distribution bd(missing_prob);
  auto pos = remove_if(
    begin(retval), end(retval),
    [&](auto const&) { return bd(re_bd); }
  );
  retval.erase(pos, end(retval));

  return retval;
}

//=============================================================================

std::string generate_random_fname()
{
  using namespace std;

  static constexpr string_view chars{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-" };
  static default_random_engine re_chars{ random_device{}() };
  static default_random_engine re_fname_len{ random_device{}() };
  static uniform_int_distribution<size_t> ud_chars(0,chars.size()-1);
  static uniform_int_distribution<size_t> ud_fname_len(5,25);

  size_t const len = ud_fname_len(re_fname_len);
  string retval;
  retval.reserve(len);
  generate_n(
    back_inserter(retval),
    len,
    []() { return chars[ud_chars(re_chars)]; }
  );
  return retval;
}

//=============================================================================

int main(int argc, char *argv[])
{
  namespace fs = std::filesystem;
  using namespace std;

  if (argc != 2)
  {
    cerr 
      << "Usage: " << argv[0] << " <path>\n"
         "        <path> is required to not exist.\n"
    ;
    return 1;
  }
  
  try
  {
    fs::path basedir{argv[1]};

    if (fs::exists(basedir))
    {
      cerr << "ERROR: <path> argument must not exist.\n";
      return 2;
    }

    fs::create_directory(basedir);

    // set the current directory to basedir...
    fs::current_path(basedir);

    // generate data with at most  decks from one company...
    auto const data = generate_input(10);

    // up to N cards can be in a single file...
    random_device rd;
    default_random_engine re(rd());
    uniform_int_distribution<size_t> ud(1,40);

    // determine fnames and the number of cards for each file...
    vector<string> fnames;
    vector<size_t> ncards;
    for (size_t left = data.size(); left != 0; )
    {
      size_t const len = ud(re);
      if (len > left)
      {
        ncards.push_back(left);
        left = 0;
      }
      else
      {
        ncards.push_back(len);
        left -= len;
      }
      fnames.emplace_back(generate_random_fname());
    }

    // data.size() should equal the sum of the number of card lengths...
    assert(accumulate(begin(ncards), end(ncards), 0) == data.size());

    // output data to fnames...
    auto d = begin(data);
    auto d_end = end(data);
    auto f = begin(fnames);
    auto n = begin(ncards);
    for (; d != d_end; ++f, ++n)
    {
      ofstream out(*f);
      for (size_t cnt = *n; cnt != 0; ++d, --cnt)
        out << *d;
      out << '\n';
    }
  }
  catch (std::exception const& e)
  {
    cerr << "FATAL_EXCEPTION: " << e.what() << '\n';
    return 126;
  }
  catch (...)
  {
    cerr << "FATAL_EXCEPTION: Unknown exception occurred.\n";
    return 127;
  }
}

//=============================================================================

