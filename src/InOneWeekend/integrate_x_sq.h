#include "rtweekend.h"
#include <iostream>
#include <iomanip>

double icd(double d) {
    return 2.0 * d;
}

double pdf(double x) {
    return 0.5;
}

int main() {
    int a = 0;
    int b = 2;
    int N = 1000000;
    auto sum = 0.0;

    for (int i = 0; i < N; i++) {
        auto x = icd(random_double());
        sum += x * x / pdf(x);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << (sum / N) << '\n';

    sum = 0;

    //Sin5 
    for (int i = 0; i < N; i++) {
        auto x = random_double(a, b);
        sum += std::pow(std::sin(x), 5.0);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << (b - a) * (sum / N) << '\n';

    sum = 0;


    //ln(sin)
    for (int i = 0; i < N; i++) {
        auto x = random_double(a, b);
        sum += std::log(std::sin(x));
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << (b - a) * (sum / N) << '\n';
}