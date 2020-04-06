#include <iostream>
#include <future>
#include <thread>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <functional>
#include <vector>
#include <iterator>

class Timer {
public:
	Timer() : m_begin(std::chrono::steady_clock::now()), duration(0), is_work(true) {}

	~Timer() {
		if (is_work)
			this->stop();
		std::cout << duration << " milliseconds" << std::endl;
	}

	int stop() {
		auto end = std::chrono::steady_clock::now();
		duration += std::chrono::duration_cast<std::chrono::milliseconds> (end - m_begin).count();
		is_work = false;
		return duration;
	}

	void go() {
		m_begin = std::chrono::steady_clock::now();
		is_work = true;
	}

private:
	std::chrono::steady_clock::time_point m_begin;
	int duration;
	bool is_work;
};

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
	//std::ofstream fout("./out3.txt");
	/*for (int i = 1000; i < 50000; i += 2000) {
		fout << i << "    ";
		std::vector<int> nums(i);
		std::iota(nums.begin(), nums.end(), 1);
		Timer t;
		for_each_parallel(nums.begin(), nums.end(), [](int& n) { n++; });
		fout << t.stop() << "    ";
		Timer t1;
		for_each(nums.begin(), nums.end(), [](int& n) { n--; });
		fout << t1.stop() << std::endl;
	}*/
	/*for (int i = 1000; i < 20000; i += 1000) {
		fout << i << "    ";
		std::vector <int> nums(i);
		std::vector <int> res(i);
		std::iota(nums.begin(), nums.end(), 1);
		Timer t;
		std::partial_sum(nums.begin(), nums.end(), res.begin());
		fout << t.stop() << "    ";
		Timer t1;
		std::inclusive_scan(nums.begin(), nums.end(), res.begin());
		fout << t1.stop() << std::endl;
	}*/
	/*std::vector<int> a{ 0, 1, 2, 3, 4 };
	std::vector<int> b{ 5, 4, 2, 3, 1 };

	int r1 = std::inner_product(a.begin(), a.end(), b.begin(), 0);
	int r1 = std::transform_reduce(a.begin(), a.end(), b.begin(), 0);
	std::cout << "Inner product of a and b: " << r1 << '\n';
	fout.close();*/
}