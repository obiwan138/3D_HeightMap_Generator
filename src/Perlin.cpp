#include "Perlin.hpp"
#include <stddef.h>
#include <cmath>
#include <iostream>

void perlin1DFull(std::vector<double> &surface, std::vector<double> &t, std::vector<double> gradient, int resolution) {
    double n0, n1, u, i, p, fade;
    size_t g0, g1;
    for (size_t idx = 0; idx < surface.size(); idx++) {
        p = idx / resolution + (idx % (size_t) resolution) / (double) resolution;
        t[idx] = p;
        u = p - floor(p);
        n0 = gradient[floor(p)] * u;
        n1 = gradient[ceil(p)] * (u-1);
        fade = easeCurve<double>(u);
        surface[idx] = n0 + fade * (n1-n0);
    }
}

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
    std::srand(static_cast<uint64_t>(seed) ^ static_cast<uint64_t>(x));
    std::srand(static_cast<uint64_t>(std::rand()) ^ static_cast<uint64_t>(y));
    uint64_t perm = std::rand() % 8;
    return glm::vec2(cos(M_PI/8 * perm), sin(M_PI/8 * perm));
}

double perlin1D(double x, int64_t seed) {
    double p = x;
    double u = p - floor(p);
    double n0 = gradient1D(seed, floor(p)) * u;
    double n1 = gradient1D(seed, ceil(p)) * (u-1);
    double fade = easeCurve<double>(u);
    return n0 + fade * (n1-n0);
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