#ifndef SEARCH_H
#define SEARCH_H

#include <iostream>
#include <unordered_map>
#include <variant>

#include "threadpool.h"
#include "sync.h"
#include "task.h"
#include "branch.h"
#include "recycler.h"

#include "deck.h"
#include "combinatorics.h"
#include "mull_strats.h"
#include "mana_sim.h"

struct SearchSystem {
	Threadpool threadpool = Threadpool(16);

	struct SearchResult {
		Deck deck = {};
		double mana;
	};

	Task<SearchResult> deckSim(const Deck deck, SimSettings settings){
		std::mt19937 gen(std::random_device{}());
		const auto play_order = settings.play_order;

		if(play_order == PlayOrder::Random){
			settings.general_iterations = settings.general_iterations / 2;

			settings.play_order = PlayOrder::Play;
			auto strat_play = optimalMulliganStrategy(deck, gen, settings);
			auto mana_play = avgManaSpentGeneral(deck, strat_play, gen, settings);

			settings.play_order = PlayOrder::Draw;
			auto strat_draw = optimalMulliganStrategy(deck, gen, settings);
			auto mana_draw = avgManaSpentGeneral(deck, strat_draw, gen, settings);
			
			double avg_mana = (mana_play + mana_draw) / 2;
			co_return {deck, avg_mana};

		}

		else{
			auto strat = optimalMulliganStrategy(deck, gen, settings);
			auto mana = avgManaSpentGeneral(deck, strat, gen, settings);
			co_return {deck, mana};	
		}
	}


	Task<Deck> hillclimbSearch(const Deck init_deck, SimSettings settings){
		auto plus_one = [](auto i){return i+1;};
		auto minus_one = [](auto i){return std::max(0,i-1);};

		std::unordered_map<Deck, double> searched_decks = {};

		Deck last_best_deck = {};
		Deck best_deck = init_deck;
		double best_mana = 0;

		while(last_best_deck != best_deck){
			last_best_deck = best_deck;
			std::array<uint8_t, 8> min;
			std::array<uint8_t, 8> max;

			std::ranges::transform(best_deck.curve, min.begin(), minus_one);
			std::ranges::transform(best_deck.curve, max.begin(), plus_one);

			//min[4] = 0;
			//min[5] = 0;
			min[6] = 0;
			min[7] = 0;


			//max[4] = 0;
			//max[5] = 0;
			max[6] = 0;
			max[7] = 0;

			using Branch = std::variant<std::monostate, Branch<SearchResult, Threadpool>>;
			std::array<Branch, 8> branches{};
			Recycler recycler{branches};

			for(auto neighbour : bounded_compositions(best_deck.size(), min, max, 1)){
				Deck deck = {neighbour};
				if(searched_decks.contains(deck)){continue;}

				auto recycled_slot = recycler.emplace(co_await threadpool.branch(deckSim(deck, settings)));
				if(recycled_slot.index() != 1){
					continue;
				}

				auto deck_result = co_await std::get<1>(recycled_slot);	
				std::cout << deck_result.deck << " ";
				std::cout << " mana: " << deck_result.mana << " ";
				if(deck_result.mana > best_mana){
					best_deck = deck_result.deck;
					best_mana = deck_result.mana;
					std::cout << "*";
				}
				std::cout << "\n";
				searched_decks.try_emplace(deck_result.deck, deck_result.mana);
			}

			for(auto& branch : branches){
				if(branch.index() != 1){
					continue;	
				}
				auto deck_result = co_await std::get<1>(branch);	
				std::cout << deck_result.deck << " ";
				std::cout << " mana: " << deck_result.mana << " ";
				if(deck_result.mana > best_mana){
					best_deck = deck_result.deck;
					best_mana = deck_result.mana;
					std::cout << "*";
				}
				std::cout << "\n";
				searched_decks.try_emplace(deck_result.deck, deck_result.mana);
			}

		}
		std::cout << "Best: " << best_deck << " mana: " << best_mana << "\n";
		co_return best_deck;
	}

	Sync<int> entry(){
		Deck deck = {24,10,13,11,2,0,0,0};
		
		SimSettings settings = {
			.max_turns = 7,
			.play_order = PlayOrder::Random
		};

		co_await hillclimbSearch(deck, settings);

		//std::mt19937 gen(std::random_device{}());
		//auto strat = optimalMulliganStrategy(deck, 5, gen);
		//std::cout << strat << "\n";
		co_return 0;
	}
};

#endif
