#ifndef HAND_H
#define HAND_H

#include <array>
#include <algorithm>
#include <ranges>
#include <functional>
#include <ostream>

struct Hand {
	std::array<uint8_t, 8> curve = {};

	bool try_play(std::array<uint8_t, 8> to_play){
		bool playable = std::ranges::equal(curve, to_play, std::greater_equal{});
		if(!playable){return false;}
		std::ranges::transform(curve, to_play, curve.begin(), std::minus{});
		return true;
	}

};

std::ostream& operator<< (std::ostream& os, const Hand& hand){
	for(int i : hand.curve){
		os << i << " ";
	}
	return os;
}

#endif
