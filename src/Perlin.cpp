#include "Perlin.hpp"
#include <stddef.h>
#include <cmath>
#include <iostream>

double gradient1D(int64_t seed, int x) {
    std::srand(static_cast<uint64_t>(seed) ^ static_cast<uint64_t>(x));
    uint64_t perm = std::rand();
    if (perm % 4 == 0)
        return -1;
    else if (perm % 4 == 1)
        return -0.5;
    else if (perm % 4 == 2)
        return 0.5;
    else
        return 1;
}


glm::vec2 gradient2D(int64_t seed, int x, int y) {
    std::srand(static_cast<uint64_t>(seed) ^ static_cast<uint64_t>(y));
    std::srand(static_cast<uint64_t>(std::rand()) ^ static_cast<uint64_t>(x));
    int N = 4;
    uint64_t perm = std::rand() % N;
    return glm::vec2(cos(M_PI/N * perm), sin(M_PI/N * perm));
}

double perlin1D(double x, int64_t seed) {
    double p = x;
    double u = p - floor(p);
    double n0 = gradient1D(seed, floor(p)) * u;
    double n1 = gradient1D(seed, ceil(p)) * (u-1);
    double fade = easeCurve<double>(u);
    return n0 + fade * (n1-n0);
}

double fractalPerlin1D(double x, int64_t seed, int octaves, double freqStart, double freqRate, double ampRate) {
    double y = 0;
    double freq = freqStart;
    double amplitude = 1;
    for (int k = 0; k < octaves; k++) {
        y += amplitude * perlin1D(x*freq, seed);
        amplitude *= ampRate;
        freq *= freqRate;
    }
    return y;
}

double perlin2D(double x, double y, int64_t seed) {
    double u = x - floor(x);
    double v = y - floor(y);
    double n00 = glm::dot(gradient2D(seed, floor(x), floor(y)), glm::vec2(u, v));
    double n10 = glm::dot(gradient2D(seed, floor(x)+1, floor(y)), glm::vec2(u-1, v));
    double n01 = glm::dot(gradient2D(seed, floor(x), floor(y)+1), glm::vec2(u, v-1));
    double n11 = glm::dot(gradient2D(seed, floor(x)+1, floor(y)+1), glm::vec2(u-1, v-1));
    double nx0 = n00 * (1 - easeCurve<double>(u)) + n10 * easeCurve<double>(u);
    double nx1 = n01 * (1 - easeCurve<double>(u)) + n11 * easeCurve<double>(u);
    return nx0 * (1 - easeCurve<double>(v)) + nx1 * easeCurve<double>(v);
}

double fractalPerlin2D(double x, double y, int64_t seed, int octaves, double freqStart, double freqRate, double ampRate) {
    double height = 0;
    double freq = freqStart;
    double amplitude = 1;
    for (int k = 0; k < octaves; k++) {
        height += amplitude * perlin2D(x*freq, y*freq, seed);
        amplitude *= ampRate;
        freq *= freqRate;
    }
    return height;
}