#ifndef DECKHAND_H
#define DECKHAND_H

#include "deck.h"
#include "hand.h"
#include <random>
#include <numeric>

struct DeckHand {
	Deck deck = {};
	Hand hand = {};

	int draw(std::mt19937& gen){
		int card_drawn = deck.draw(gen);
		if(card_drawn == -1){return -1;};
		hand.curve.at(card_drawn)++;
		return card_drawn;
	}

	bool try_play(std::array<uint8_t, 8> to_play){
		return hand.try_play(to_play);
	}

	void drawSeven(std::mt19937& gen){
		for(int i = 0; i < 7; i++){
			draw(gen);
		}
	}

	int total_mana_in_hand(){
		std::array<uint8_t, 8> idx = {0,1,2,3,4,5,6,7};
		return std::inner_product(idx.begin(), idx.end(), hand.curve.begin(), int{0});
	}
};

#endif
