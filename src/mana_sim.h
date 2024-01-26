#ifndef MANA_SIM_H
#define MANA_SIM_H

#include "mulligan.h"
#include "deckhand.h"
#include "combinatorics.h"

double manaSpent(DeckHand deck_hand, int max_turns, std::mt19937& gen){
	int cumulative_mana = 0;
	int onboard_value = 0;
	//double mana_spent = 0;	
	int max_mana = 0;

	for(int turn = 1; turn <= max_turns; turn++){
		//draw for turn
		if(turn != 1){
			deck_hand.draw(gen);
		}


		//play land for turn
		std::array<uint8_t, 8> land_for_turn = {1,0,0,0,0,0,0,0};
		if(deck_hand.try_play(land_for_turn)){
			max_mana++;
		}
		
		//total mana in hand
		int total_mana_in_hand = deck_hand.total_mana_in_hand();
		int most_try_spend = std::min(max_mana, total_mana_in_hand);

		int turn_mana = max_mana;
		
		//try to play cards in hand
		for(int try_to_spend = most_try_spend; try_to_spend > 0; try_to_spend--){
			for(auto way_to_spend : partitions<uint8_t, 8>(try_to_spend)){
				if(deck_hand.try_play(way_to_spend)){
					onboard_value += try_to_spend;
					//mana_spent += ((double)try_to_spend/(double)turn);
					turn_mana -= try_to_spend;
					break;	
				}
			}

			if(turn_mana == 0){break;}
		}

		cumulative_mana += onboard_value;
	}
	
	return cumulative_mana;
	//return mana_spent;
}

double avgManaSpentGeneral(Deck deck, MulliganStrategy& strat, int max_turns, unsigned long iterations, std::mt19937& gen, uint8_t hand_size = 7){
	double total_mana_spent = 0;
	for(unsigned long  i = 0; i < iterations; i++){
		auto deck_hand = initWithMulligan(deck, strat, gen, hand_size);
		total_mana_spent += manaSpent(deck_hand, max_turns, gen);
	}
	return total_mana_spent/iterations;
}

double avgManaSpentSpecific(DeckHand deck_hand, int max_turns, unsigned long iterations, std::mt19937& gen){
	double total_mana_spent = 0;
	for(unsigned long  i = 0; i < iterations; i++){
		total_mana_spent += manaSpent(deck_hand, max_turns, gen);
	}
	return total_mana_spent/iterations;
}

#endif
