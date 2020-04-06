#include <iostream>
#include <random>
#include <chrono>


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 2.0);
    double x, y;
    int in = 0;
    int all = 100000;
    for (int n = 0; n < all; ++n) {
        x = dis(gen);
        y = dis(gen);
        if (((x - 1)*(x - 1) + (y - 1)*(y - 1)) < 1.0)
            ++in;
    }
    std::cout.precision(7);
    std::cout << 4.0 * in / all;
    return 0;
}