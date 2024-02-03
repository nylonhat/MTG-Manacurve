#ifndef MULL_STRATS_H
#define MULL_STRATS_H

#include "mulligan.h"
#include "mana_sim.h"

MulliganStrategy optimalMulliganStrategy(const Deck deck, std::mt19937& gen, SimSettings settings){

	MulliganStrategy strat = {};

	std::array<uint8_t, 8> min = {0,0,0,0,0,0,0,0};
	std::array<uint8_t, 8> max = deck.curve;
	
	double avg_mana_next_mull = 0;

	for(uint8_t hand_size = 4; hand_size <= 7; hand_size++){
			
		for(auto opening_curve : bounded_compositions(7, min, max, 1)){
			double best_mana_spent = 0;
			std::array<uint8_t, 8> best_to_bottom = {};
			
			for(auto to_bottom_curve : bounded_compositions(7 - hand_size, min, opening_curve, 1)){
				DeckHand deck_hand = {};
				std::ranges::transform(deck.curve, opening_curve, deck_hand.deck.curve.begin(), std::minus{});
				deck_hand.hand.curve = opening_curve;
				deck_hand.try_play(to_bottom_curve);
				double avg_mana = avgManaSpentSpecific(deck_hand, gen, settings);
				if(avg_mana >= best_mana_spent){
					best_mana_spent = avg_mana;
					best_to_bottom = to_bottom_curve;
				}
			}

			auto hand_to_mull = HandToMulligan{hand_size, opening_curve};
			auto mull_choice = MulliganChoice{(best_mana_spent >= avg_mana_next_mull), best_to_bottom};
			strat.map.try_emplace(hand_to_mull, mull_choice);
			//std::cout << hand_to_mull << mull_choice << best_mana_spent << "\n";
		}
			
		//test with this hand size
		if(hand_size == 7){break;}
		avg_mana_next_mull = avgManaSpentGeneral(deck, strat, gen, settings, hand_size);
		//std::cout << avg_mana_next_mull << "\n";
	}
	
	return strat;
}


#endif
