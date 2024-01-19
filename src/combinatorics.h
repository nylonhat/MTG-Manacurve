#ifndef COMBINATORICS_H
#define COMBINATORICS_H

#include <array>
#include <ranges>
#include <algorithm>
#include "__generator.hpp" 

template<typename A>
std::generator<A> bounded_compositions(const int sum, const A min, const A max, const int stride){
	A out = {};
	A remains = {};
	
	const auto size = out.size();

	if(size == 0){
		co_yield out;
		co_return;
	}

	out = min;
	remains.fill(sum);

	while(true){
		for(size_t depth = 0; out[depth] <= remains[depth]; depth++){
			//base case
			if(depth == (size - 1)){
				out[depth] = remains[depth];
				if(out[depth] > max[depth]){break;}
				co_yield out;
				break;
			}
			
			//recursive case
			if(out[depth] > max[depth]){break;}
			remains[depth+1] = remains[depth] - out[depth]; 
		}
	
		//forward progress
		for(size_t depth = size - 1; (out[depth]+=stride) > remains[depth]; out[depth]=min[depth], depth--){
			//termination case
			if(depth == 0){co_return;}
		}
	}

	co_return;
}

template<typename T, size_t N>
std::generator<std::array<T,N>> partitions(int sum){
	std::array<T, N> output = {};
	output.at(sum) = 1;
	co_yield output;
	
	auto any = [](auto i){return i>0;};
	auto found_iter = std::ranges::find_if(std::views::drop(output,2), any);
	while(found_iter != output.end()){
		std::array<T, N> remains = {};
		auto idx = std::ranges::distance(output.begin(), found_iter);
		output.at(idx)--;
		remains.at(sum) = sum;

		for(size_t b = sum-1; b > 0; b--){
			remains.at(b) = remains.at(b+1) - (output.at(b+1) * (b+1));
			if(b >= idx){continue;}
			output.at(b) = remains.at(b)/b;
		}

		co_yield output;
		found_iter = std::ranges::find_if(std::views::drop(output,2), any);
	}

	co_return;
}


#endif
