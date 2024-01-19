#ifndef DECK_H
#define DECK_H

#include <random>
#include <numeric>
#include <ostream>

struct Deck {
	std::array<uint8_t,8> curve = {};

	int draw(std::mt19937& gen){
		std::array<uint8_t,8> cdf;
		std::partial_sum(curve.cbegin(), curve.cend(), cdf.begin());
		if(cdf.back() == 0){return -1;}
		std::uniform_int_distribution<> distrib(1,cdf.back());
		int rand = distrib(gen);
		auto in_bucket = [=](auto i){return i >= rand;};
		auto found_iter = std::ranges::find_if(cdf, in_bucket);	
		int index = std::ranges::distance(cdf.begin(), found_iter);
		curve[index]--;
		return index;
	}

	int size(){
		return std::accumulate(curve.begin(), curve.end(), int{0});
	}
	
	auto operator<=>(const Deck&) const = default;

};

template<>
struct std::hash<Deck>{
	std::size_t operator()(const Deck& deck) const{
		return *reinterpret_cast<const uint64_t*>(deck.curve.data());
	}
};

std::ostream& operator<< (std::ostream& os, const Deck& deck){
	os << "Deck: ";
	for(int i : deck.curve){
		os << i << " ";
	}
	return os;
}



#endif
