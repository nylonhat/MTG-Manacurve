#ifndef MULLIGAN_H
#define MULLIGAN_H

#include <iostream>
#include <ostream>
#include <array>
#include <unordered_map>
#include "deckhand.h"

struct HandToMulligan {
	uint8_t start_size = 7;
	std::array<uint8_t, 8> curve = {};

	auto operator<=>(const HandToMulligan&) const = default;	
};

std::ostream& operator<< (std::ostream& os, const HandToMulligan& hand){
	os << "mull to " << (int)hand.start_size << ":\t";
	for(int i : hand.curve){
		os << i << " ";
	}
	return os;
}

template<>
struct std::hash<HandToMulligan>{
	std::size_t operator()(const HandToMulligan& hand) const{
		uint32_t h = hand.start_size;
		for(uint8_t count : hand.curve){
			h = h << 3;
			h |= count;
		}
		return h;
	}
};

struct MulliganChoice {
	bool keep = true;
	std::array<uint8_t, 8> to_bottom = {};
};

std::ostream& operator<< (std::ostream& os, const MulliganChoice& choice){
	os << std::boolalpha << choice.keep << " " << std::noboolalpha;
	for(int i : choice.to_bottom){
		os << i << " ";
	}
	return os;
}

struct MulliganStrategy {
	std::unordered_map<HandToMulligan, MulliganChoice> map = {};	
};

std::ostream& operator<< (std::ostream& os, const MulliganStrategy& strat){
	for(auto [hand, choice] : strat.map){
		std::cout << hand << "  " << choice << "\n";
	}
	return os;
}

DeckHand initWithMulligan(Deck deck, MulliganStrategy& strat, std::mt19937& gen, uint8_t max_hand_size = 7){
	for(uint8_t start_hand_size = max_hand_size; start_hand_size > 0; start_hand_size--){
		DeckHand deck_hand = {deck,{}};

		deck_hand.drawSeven(gen);
		
		auto hand_to_mull = HandToMulligan{start_hand_size, deck_hand.hand.curve};
		auto mull_choice = strat.map.at(hand_to_mull);

		if(!mull_choice.keep){continue;}

		//discard bottomed cards from mulligan
		deck_hand.try_play(mull_choice.to_bottom);	
		return deck_hand;
	}
	//safetly catch
	std::cout << "safety catch!\n";
	return {};
}



#endif
