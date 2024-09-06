﻿#include "rtweekend.h"
#include <iostream>
#include <iomanip>

double icd(double d) {      //randomly samble values from the distribution
    return 8.0 * std::pow(d, 1.0 / 3.0);
}

double pdf(double x) {
    return x / 2.0;
}

int main() {
    int a = 0;
    int b = 2;
    int N = 1;
    auto sum = 0.0;

    for (int i = 0; i < N; i++) {
        auto z = random_double();
        if (z == 0.0)  // Ignore zero to avoid NaNs
            continue;

        auto x = icd(z);
        sum += x * x / pdf(x);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << (sum / N) << '\n';
}