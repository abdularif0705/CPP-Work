#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "a4-provided.hpp"
#include "a5-provided.hpp"
#include "a4-include.hpp"

// Define a strong ordering operator for playing card companies based on their name
std::strong_ordering operator<=>(playing_card_company const &lhs, playing_card_company const &rhs) {
  return lhs.name() <=> rhs.name();
}

// A structure that holds a playing card company and a playing card
struct CardCompany {
  playing_card_company company;
  playing_card card;
};

// Read a playing card company and playing card from an input stream, and return them as a CardCompany object
std::optional<CardCompany> read_card_company(std::istream &is) {
  std::optional<playing_card> playCard = read_playing_card(is);
  playing_card_company company;
  if (playCard && (is >> company)) {
    return CardCompany{company, playCard.value()};
  } else return std::nullopt;
}

// Generate a full deck of playing cards (including jokers) and return them as a set
std::set<playing_card> generate_full_deck() {
  std::set<playing_card> deck;
  for (card_face face: {card_face::ace, card_face::two, card_face::three, card_face::four, card_face::five,
                        card_face::six, card_face::seven, card_face::eight, card_face::nine, card_face::ten,
                        card_face::knight, card_face::jack, card_face::queen, card_face::king}) {
    for (card_suit suit: {card_suit::spades, card_suit::hearts, card_suit::diamonds, card_suit::clubs}) {
      deck.insert({face, suit});
    }
  }
  deck.insert({card_face::red_joker});
  deck.insert({card_face::white_joker});
  return deck;
}

// Main function
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path>\n";
    return 1;
  }

  // Read all playing cards from files in the specified directory, and store them in a map keyed by their company
  std::map<playing_card_company, std::vector<playing_card>> all_cards;
  for (const auto &entry: std::filesystem::directory_iterator(argv[1])) {
    std::ifstream input(entry.path().native());
    while (auto card = read_card_company(input)) {
      all_cards[card->company].push_back(card->card);
    }
  }

  // Print the total number of cards
  std::size_t total_cards = 0;
  for (const auto &entry: all_cards) {
    total_cards += entry.second.size();
  }
  std::cout << "Total Number of cards: " << total_cards << std::endl;

  // Print the number of companies and card statistics for each company
  std::cout << "Number of Companies: " << all_cards.size() << std::endl;
  for (const auto &entry: all_cards) {
    std::cout << "  " << std::quoted(entry.first.name()) << std::endl;

    // Calculate card statistics for the current company
    std::cout << std::quoted(entry.first.name()) << " card stats: \n";
    std::cout << "Total number of cards: " << entry.second.size() << std::endl;
    // Divide cards into decks
    std::vector<std::set<playing_card>> decks;
    for (const auto &card: entry.second) {
      bool inserted_card = false;
      for (auto &deck: decks) {
        auto res = deck.insert(card);
        if (res.second) { // If the card is inserted into the deck successfully
          inserted_card = true;
          break;
        }
      }
      if (!inserted_card) { // If the card is not inserted into any deck, create a new deck
        decks.push_back({});
        decks.back().insert(card);
      }
    }
    std::cout << "Total number of decks: " << decks.size() << "\n";
    std::size_t count = 1;
    for (const auto &deck: decks) {
      // Check if the deck is complete
      auto full_deck = generate_full_deck();
      std::vector<playing_card> missing;
      std::set_difference(full_deck.begin(), full_deck.end(),
                          deck.begin(), deck.end(),
                          std::back_inserter(missing));
      if (missing.empty()) {
        std::cout << "Deck " << count << " is complete.\n";
      } else {
        std::cout << "Deck " << count << " is missing the following cards:";
        for (const auto &card: missing) {
          std::cout << " " << card;
        }
        std::cout << '\n';
      }
      count++;
    }
  }
}