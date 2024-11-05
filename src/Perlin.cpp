#define _USE_MATH_DEFINES

#include "Perlin.hpp"
#include <stddef.h>
#include <cmath>
#include <iostream>

GradientNoise::GradientNoise() : gradient1(std::time(NULL)), gradient2(std::time(NULL)) {}

GradientNoise::GradientNoise(int64_t seed) : gradient1(seed), gradient2(seed) {}

GradientNoise::~GradientNoise() {}

GradientNoise::Gradient2::Gradient2(int64_t seed) : rd(), generator(rd()), distribution(0,7) {
    std::srand(static_cast<uint64_t>(seed));
}
    
glm::vec2 GradientNoise::Gradient2::at(glm::vec2 position) {
    std::pair<int, int> pos = std::pair<int, int>(position.x, position.y);
    if (_gradients.count(pos) == 0) {
        _gradients.emplace(pos, generate());
    }
    return _gradients[pos];
}

glm::vec2 GradientNoise::Gradient2::at(int x, int y) {
    std::pair<int, int> pos = std::pair<int, int>(x, y);
    if (_gradients.count(pos) == 0) {
        _gradients.emplace(pos, generate());
    }
    return _gradients[pos];
}

glm::vec2 GradientNoise::Gradient2::generate() {
    int N = 4;
    uint64_t perm = std::rand() % N;
    if (perm == 0)
        return glm::vec2(1, 0);
    else if (perm == 1)
        return glm::vec2(-1, 0);
    else if (perm == 2)
        return glm::vec2(0, 1);
    else if (perm == 3)
        return glm::vec2(0, -1);
    //return glm::vec2(cos(M_PI/N * perm), sin(M_PI/N * perm));
}

GradientNoise::Gradient1::Gradient1(int64_t seed) {
    std::srand(static_cast<uint64_t>(seed));
}
    
double GradientNoise::Gradient1::at(int x) {
    if (_gradients.count(x) == 0) {
        _gradients.emplace(x, generate());
    }
    return _gradients[x];
}

double GradientNoise::Gradient1::generate() {
    int N = 8;
    uint64_t perm = std::rand() % N;
    return cos(M_PI/N * perm);
}

double GradientNoise::perlin1D(double x) {
    double p = x;
    double u = p - floor(p);
    double n0 = gradient1.at(floor(p)) * u;
    double n1 = gradient1.at(ceil(p)) * (u-1);
    double fade = easeCurve<double>(u);
    return n0 + fade * (n1-n0);
}

double GradientNoise::fractalPerlin1D(double x, int octaves, double freqStart, double freqRate, double ampRate) {
    double y = 0;
    double freq = freqStart;
    double amplitude = 1;
    for (int k = 0; k < octaves; k++) {
        y += amplitude * perlin1D(x*freq);
        amplitude *= ampRate;
        freq *= freqRate;
    }
    return y;
}

double GradientNoise::perlin2D(double x, double y) {
    double u = x - floor(x);
    double v = y - floor(y);
    double n00 = glm::dot(gradient2.at(floor(x), floor(y)), glm::vec2(u, v));
    double n10 = glm::dot(gradient2.at(floor(x)+1, floor(y)), glm::vec2(u-1, v));
    double n01 = glm::dot(gradient2.at(floor(x), floor(y)+1), glm::vec2(u, v-1));
    double n11 = glm::dot(gradient2.at(floor(x)+1, floor(y)+1), glm::vec2(u-1, v-1));
    double nx0 = n00 * (1 - easeCurve<double>(u)) + n10 * easeCurve<double>(u);
    double nx1 = n01 * (1 - easeCurve<double>(u)) + n11 * easeCurve<double>(u);
    return nx0 * (1 - easeCurve<double>(v)) + nx1 * easeCurve<double>(v);
}

double GradientNoise::fractalPerlin2D(double x, double y, int octaves, double freqStart, double freqRate, double ampRate) {
    double height = 0;
    double freq = freqStart;
    double amplitude = 1;
    for (int k = 0; k < octaves; k++) {
        height += amplitude * perlin2D(x*freq, y*freq);
        amplitude *= ampRate;
        freq *= freqRate;
    }
    return height;
}

void GradientNoise::fractalPerlin2D(glm::vec3& pos, int octaves, double freqStart, double freqRate, double ampRate) {
    double height = 0;
    double freq = freqStart;
    double amplitude = 1;
    for (int k = 0; k < octaves; k++) {
        pos.y += amplitude * perlin2D(pos.x*freq, pos.z*freq);
        amplitude *= ampRate;
        freq *= freqRate;
    }
    return;
}