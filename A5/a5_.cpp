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

std::strong_ordering operator<=>(playing_card_company const& lhs, playing_card_company const& rhs)
{return lhs.name() <=> rhs.name();}
struct pcc{
	playing_card_company pcm;
	playing_card card;
};
std::optional<pcc> read_pcc(std::istream& is){
	std::optional<playing_card> playCard = read_playing_card(is);
	playing_card_company company;
	if(playCard && (is >> company)){
		return pcc{company, playCard.value()};
	}
	else return std::nullopt;
}
std::set<playing_card>generate_full_deck(){
	std::set<playing_card>retval;//goes through all card faces and suits, and insert (non joker faces)
	for(card_face face: {card_face::ace,card_face::two,card_face::three,card_face::four,card_face::five,
			     card_face::six,card_face::seven,card_face::eight,card_face::nine,card_face::ten,
			     card_face::knight,card_face::jack,card_face::queen,card_face::king}){
		for(card_suit suit: {card_suit::spades,card_suit::hearts,card_suit::diamonds,card_suit::clubs}){
			retval.insert({face,suit});
		}
	}//goes through all joker faces and insert
	for(card_face face: {card_face::red_joker,card_face::white_joker}){retval.insert({face});}
	return retval;
}
int main(int argc, char*argv[]){
	if(argc!=2){std::cerr << "Usage: " << argv[0] << " <path>\n"; return 1;}

	std::map<playing_card_company,std::vector<playing_card>> all_cards;
	for(const auto& entry: std::filesystem::directory_iterator(argv[1])){
		std::ifstream input(entry.path().native());
		while(auto datum = read_pcc(input)){ all_cards[datum->pcm].push_back(datum->card);}
	}

	std::size_t int_cards=0;
	for(const auto& entry: all_cards){int_cards+=entry.second.size();}
	std::cout << "Total Number of cards: " << int_cards << std::endl;

	std::cout << "Number of Companies: " << all_cards.size() << std::endl;
	for(const auto& entry: all_cards){std::cout << "  " << std::quoted(entry.first.name()) << std::endl;

	for(const auto& entry: all_cards){
		std::cout << "\n";
		std::cout << std::quoted(entry.first.name()) << "card stats: \n";
		std::cout << "Total number of cards: " << entry.second.size() << std::endl;

		std::vector<std::set<playing_card>> decks;
		for(const auto& card:entry.second){
			bool inserted_card = false; //this will
			for(auto& deck:decks){
				auto res = deck.insert(card);
				if(res.second){//implies that the card is in the deck already
					inserted_card = true;
					break;
				}
			}
			if(!inserted_card){//checks if the inserted_card is not true. Meaning, card not inserted
				decks.push_back({});
				decks.back().insert(card);
			}
		}
		std::cout << "Total number of decks: " << decks.size() << "\n";
		std::size_t count = 1;
		for(const auto& deck:decks){
			auto full_deck = generate_full_deck();
			std::vector<playing_card> missing;
			std::set_difference(full_deck.begin(),full_deck.end(),
					    deck.begin(),deck.end(),
					    std::back_inserter(missing));
			if(missing.empty()){std::cout << " Deck " << count << " is not missing any cards.\n";}
			//check if there are no cards in missing
			else{
				std::cout << " Deck" << count << " is missing: ";
				for(const auto& card: missing){std::cout << " " << card;}
				std::cout << '\n';
			}
			count++;
		}
	}
}

}
