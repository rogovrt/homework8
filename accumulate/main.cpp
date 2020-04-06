#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>

class Timer {
public:
	Timer() : m_begin(std::chrono::steady_clock::now()), duration(0), is_work(true) {}

	~Timer() {
		if (is_work)
			this->stop();
		std::cout << duration << " milliseconds" << std::endl;
	}

	void stop() {
		auto end = std::chrono::steady_clock::now();
		duration += std::chrono::duration_cast<std::chrono::milliseconds> (end - m_begin).count();
		is_work = false;
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

template < typename Iterator, typename T >
struct accumulate_block
{
	T operator()(Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};

template < typename Iterator, typename T >
T parallel_accumulate(Iterator first, Iterator last, T init, int num_threads)
{
	const std::size_t length = std::distance(first, last);

	if (!length)
		return init;


	const std::size_t block_size = length / num_threads;

	std::vector < std::future < T > > futures(num_threads - 1);
	std::vector < std::thread >		  threads(num_threads - 1);

	Iterator block_start = first;
	try {
		for (std::size_t i = 0; i < (num_threads - 1); ++i)
		{
			Iterator block_end = block_start;
			std::advance(block_end, block_size);

			std::packaged_task < T(Iterator, Iterator) > task{
				accumulate_block < Iterator, T >() };

			futures[i] = task.get_future();
			threads[i] = std::thread(std::move(task), block_start, block_end);

			block_start = block_end;
		}
		std::for_each(threads.begin(), threads.end(),
			std::mem_fn(&std::thread::join));
	}
	catch (...) {
		for (unsigned long i = 0; i < (num_threads - 1); ++i)
		{
			if (threads[i].joinable())
				threads[i].join();
		}
		throw;
	}

	T last_result = accumulate_block < Iterator, T >()(block_start, last);

	T result = init;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main(int argc, char** argv)
{
	std::vector < int > v(100000);

	std::iota(v.begin(), v.end(), 1);

	for (int i = 1; i < 20; ++i) {
		std::cout << i + 1 << " ";
		Timer t;
		parallel_accumulate(v.begin(), v.end(), 0, i);
		t.stop();
	}

	system("pause");

	return EXIT_SUCCESS;
}