#pragma once
#include <vector>
#include <ctime>
#include <glm/glm.hpp>

template <typename T>
T easeCurve(T t) { // f(t) = 6 * t^6 - 15 * t^5 + 10 * t^3 
    return ((6 * t - 15) * t + 10) * t * t * t;
}

class LFSR {
public:
    LFSR(uint64_t seed) : state(seed) { }

    uint64_t step();

    uint64_t generate(size_t num_bits);

private:
    uint64_t state;
};

class RNG {
public:
    RNG() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    int generate(int, int);
};

void perlin1DFull(std::vector<double> &surface, std::vector<double> &t, std::vector<double> gradient, int resolution);

double perlin2D(double x, double y, int64_t seed);

double gradient1D(int64_t seed, int x);

glm::vec2 gradient2D(int64_t seed, int x, int y);

double perlin1D(double x, int64_t seed);

double perlin2D(double x, double y, int64_t seed);

void perlin2D(glm::vec3& pos, int64_t seed);