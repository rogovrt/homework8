#include <iostream>
#include <future>
#include <thread>
#include <algorithm>

template <typename Iter, typename Func>
void for_each_parallel(Iter first, Iter second, Func f) {
	int len = std::distance(first, second);
	if (len == 0)
		return;
	int block_size = 25;
	if (len < block_size)
		std::for_each(first, second, f);
	else {
		Iter middle = first + len / 2;
		std::future <void> first_half = std::async
		(&for_each_parallel <Iter, Func>, first, middle, f);
		for_each_parallel(middle, second, f);
		first_half.get();
	}
}

int main() {
	std::vector<int> nums{ 3, 4, 2, 9, 15, 267 };

	std::cout << "before: ";
	for (auto n : nums) {
		std::cout << n << " ";
	}
	std::cout << '\n';

	for_each_parallel(nums.begin(), nums.end(), [](int& n) { n++; });

	std::cout << "after:  ";
	for (auto n : nums) {
		std::cout << n << " ";
	}
}