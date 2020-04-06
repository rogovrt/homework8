#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <future>

int part(int block_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 2.0);
    int in = 0;
    for (int n = 0; n < block_size; ++n) {
        double x = dis(gen);
        double y = dis(gen);
        if (((x - 1) * (x - 1) + (y - 1) * (y - 1)) < 1.0)
            ++in;
    }
    return in;
}

int main() {
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0)
        num_threads = 2;
    int all = 100000;
    int block_size = all / num_threads;
    std::vector < std::future < int > > futures(num_threads - 1);
    std::vector < std::thread > threads(num_threads - 1);
    try {   
        for (int i = 0; i < (num_threads - 1); ++i) {
            std::packaged_task <int(int)> task(part);
            futures[i] = task.get_future();
            threads[i] = std::thread(std::move(task), block_size);
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
    int result = part(all - block_size * (num_threads - 1));
    for (int i = 0; i < (num_threads - 1); ++i)
        result += futures[i].get();
    std::cout << 4.0 * result / all;
    return 0;
}