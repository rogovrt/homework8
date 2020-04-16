#include <iostream>
#include <future>
#include <thread>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <functional>
#include <vector>
#include <iterator>
#include <execution>
#include <iomanip>

class Timer {
public:
	Timer() : m_begin(std::chrono::steady_clock::now()), duration(0), is_work(true) {}

	~Timer() {
		if (is_work)
			this->stop();
		//fout << duration << " milliseconds" << std::endl;
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

int main() {
	std::ofstream fout("./results.txt");
	
	fout << "FOR_EACH\n";
	fout << std::setw(10) << "elems" << std::setw(10) << "seq" << std::setw(10) << "par\n";
	for (int i = 1000000; i < 20000000; i += 1000000) {
		fout << std::setw(8) << i;
		std::vector<int> nums(i);
		std::iota(nums.begin(), nums.end(), 1);
		Timer t;
		for_each(std::execution::seq, nums.begin(), nums.end(), [](int& n) { n++; });
		fout << std::setw(10) << t.stop();
		Timer t1;
		for_each(std::execution::par, nums.begin(), nums.end(), [](int& n) { n--; });
		fout << std::setw(10) << t1.stop() << std::endl;
	}
	fout << std::endl;

	auto f = [](const auto lhs, const auto rhs) {
		auto result = 0.0;
		for (std::size_t i = 0U; i < 10U; ++i) {
			result = std::sin(lhs + rhs + result) * std::cos(std::sin(lhs + rhs + result)); // выражение в числах с плавающей точкой
		}
		return result;
	};

	fout << "PARTIAL_SUM AND INCLUSIVE_SCAN\n";
	fout << std::setw(10) << "elems" << std::setw(10) << "seq" << std::setw(10) << "par\n";
	for (int i = 1000000; i < 20000000; i += 1000000) {
		fout << std::setw(10) << i;
		std::vector <int> nums(i);
		std::vector <int> res(i);
		std::iota(nums.begin(), nums.end(), 1);
		Timer t;
		std::partial_sum(nums.begin(), nums.end(), res.begin(), f);
		fout << std::setw(10) << t.stop();
		Timer t1;
		std::inclusive_scan(std::execution::par, nums.begin(), nums.end(), res.begin(), f);
		fout << std::setw(10) << t1.stop() << std::endl;
	}
	fout << std::endl;

	fout << "INNER_PRODUCT AND TRANSFORN REDUCE\n";
	fout << std::setw(10) << "elems" << std::setw(10) << "seq" << std::setw(10) << "par\n";
	for (int i = 1000000; i < 20000000; i += 1000000) {
		fout << std::setw(10) << i;
		std::vector <int> a(i);
		std::vector <int> b(i);
		std::iota(a.begin(), a.end(), 1);
		std::iota(b.begin(), b.end(), 2);
		Timer t;
		std::inner_product(a.begin(), a.end(), b.begin(), 0);
		fout << std::setw(10) << t.stop();
		Timer t1;
		std::transform_reduce(std::execution::par, a.begin(), a.end(), b.begin(), 0);
		fout << std::setw(10) << t1.stop() << std::endl;
	}
	return 0;
}