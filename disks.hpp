//Hammad Qureshi
//Brian Tan

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// TODO
#include <functional>
#include <iostream>

using namespace std;

enum disk_color { DISK_DARK, DISK_LIGHT };

class disk_state {
private:
	std::vector<disk_color> _colors;

public:
	disk_state(size_t light_count)
		: _colors(light_count * 2, DISK_DARK) {

		assert(light_count > 0);

		for (size_t i = 0; i < _colors.size(); i += 2) {
			_colors[i] = DISK_LIGHT;
		}

	}

	bool operator== (const disk_state& rhs) const {
		return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
	}

	size_t total_count() const {
		return _colors.size();
	}

	size_t light_count() const {
		return total_count() / 2;
	}

	size_t dark_count() const {
		return light_count();
	}

	bool is_index(size_t i) const {
		return (i < total_count());
	}

	disk_color get(size_t index) const {
		assert(is_index(index));
		return _colors[index];
	}

	void swap(size_t left_index) {
		assert(is_index(left_index));
		auto right_index = left_index + 1;
		assert(is_index(right_index));
		std::swap(_colors[left_index], _colors[right_index]);
	}

	std::string to_string() const {
		std::stringstream ss;
		bool first = true;
		for (auto color : _colors) {
			if (!first) {
				ss << " ";
			}

			if (color == DISK_LIGHT) {
				ss << "L";
			}
			else {
				ss << "D";
			}

			first = false;
		}
		return ss.str();
	}

	// Return true when this disk_state is in alternating format. That means
	// that the first disk at index 0 is light, the second disk at index 1
	// is dark, and so on for the entire row of disks.
	bool is_initialized() const {
		int flag = 0;
		for (size_t i = 0; i < _colors.size(); i += 2) {  //check from start to end, stepping by 2
			if ((_colors[i] != DISK_LIGHT) && (_colors[i + 1] != DISK_DARK))  // at each step, check adjacent disks
				flag++;
		}
		if (flag != 0) 
			return false;
		return true;

	}

	// Return true when this disk_state is fully sorted, with all dark disks
	// on the left (low indices) and all light disks on the right (high
	// indices).
	bool is_sorted() const {
		int flag = 0;	
		unsigned int half = _colors.size() / 2;	 	
		for (size_t i = 0; i < half; i++) {		// loop to check from start to halfway point
			if (_colors[i] != DISK_DARK)
				flag++;
		}
		for (size_t j = half; j < _colors.size(); j++) {   //loop to check from halfway point to the end
			if (_colors[j] != DISK_LIGHT)
				flag++;
		}
		if (flag != 0)
			return false;
		return true;
	}
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks {
private:
	disk_state _after;
	unsigned _swap_count;

public:

	sorted_disks(const disk_state& after, unsigned swap_count)
		: _after(after), _swap_count(swap_count) { }

	sorted_disks(disk_state&& after, unsigned swap_count)
		: _after(after), _swap_count(swap_count) { }

	const disk_state& after() const {
		return _after;
	}

	unsigned swap_count() const {
		return _swap_count;
	}
};

// Algorithm that sorts disks using the alternate algorithm.
sorted_disks sort_alternate(const disk_state& before) {
	disk_state _after = before;
	unsigned int _swap_count = 0;
	unsigned int runCount = 1; // run counter to keep track of which iteration the loop is in 
	
	if (_after.is_sorted())    // checks if the sequence is already sorted. if true, return as is.
		return sorted_disks(_after, _swap_count);
	else{
		if (_after.is_initialized()){                                    //checks if the sequence is initialized correctly
			for (unsigned int i = 0; i < _after.total_count() / 2; i++){ // algorithm needs to run n times
			if (runCount % 2 != 0){    				// odd iterations start at index 0, even start at index 1.
				for (unsigned int j = 0; j < _after.total_count(); j+=2){  // start at Leftmost index, go right till 
					if (_after.get(j) != _after.get(j+1)){                    // the RightMost index, stepping by 2
					_after.swap(j);
					_swap_count++;
					}
				}
			}
			else{
				for (unsigned int j = 1; j < _after.total_count() - 2; j+=2){  //start at Second Leftmost index, go right till
					if (_after.get(j) != _after.get(j+1)){                     // the Second Rightmost index, stepping by 2
					_after.swap(j);
					_swap_count++;
					}
				}
			}
			runCount++;
			}
		}
		else
			return sorted_disks(_after, _swap_count);	  // return as is if sequence is NOT initialized correctly                                                 
	}
	return sorted_disks(_after, _swap_count);
}


// Algorithm that sorts disks using the lawnmower algorithm.
sorted_disks sort_lawnmower(const disk_state& before) {
	disk_state _after = before;
	unsigned _swap_count = 0;
	unsigned int n_max_index = _after.total_count() - 1; // size of n. shrinks by 1 with each movement.
	
	if (_after.is_sorted())       // checks if the sequence is already sorted. if true, return as is.
		return sorted_disks(_after, _swap_count);
	else{
		if (_after.is_initialized()){		//checks if the sequence is initialized correctly
												//1 movement = left to right, right to left. ceiling(n/2) movements needed.
			for (unsigned int i = 0; i < _after.total_count() / 2; i++) { // OR 1 run = left to right. n runs needed.
				for (unsigned int j = i; j < n_max_index; j+=2) {    // start from Left, go right, stepping 2
				if (_after.get(j) != _after.get(j + 1)) {     // swap if adjacent disk color is different
					_after.swap(j);
					_swap_count++;
					}
				}
				n_max_index--;               // max index decreases by 1. 
				if (n_max_index < i) {      // if the max index is greater than the outer loop iteration, all swaps necessary have been done.
					for (unsigned int k = n_max_index; k > i; k-=2) {       // start from Right disk that needs swapping, go left, stepping 2
						if (_after.get(k) != _after.get(k - 1)) {  // swap if adjacent disk color is different
							_after.swap(k - 1);
							_swap_count++;
						}
					}
				}
			}
		}
		else
			return sorted_disks(_after, _swap_count); // return as is if sequence is NOT initialized correctly
	}
	return sorted_disks(_after, _swap_count);
}
