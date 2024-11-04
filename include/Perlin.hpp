#pragma once
#include <vector>
#include <ctime>
#include <glm/glm.hpp>

template <typename T>
T easeCurve(T t) { // f(t) = 6 * t^6 - 15 * t^5 + 10 * t^3 
    return ((6 * t - 15) * t + 10) * t * t * t;
}

double gradient1D(int64_t seed, int x);

glm::vec2 gradient2D(int64_t seed, int x, int y);

double perlin1D(double x, int64_t seed);

double perlin2D(double x, double y, int64_t seed);

void perlin2D(glm::vec3& pos, int64_t seed);

double fractalPerlin1D(double x, int64_t seed, 
        int octaves=8, double freqStart=0.05, double freqRate=0.5, double ampRate=0.5);

double fractalPerlin2D(double x, double y, int64_t seed, 
        int octaves=8, double freqStart=0.05, double freqRate=0.5, double ampRate=0.5);

void fractalPerlin2D(glm::vec3& pos, int64_t see, int octaves=8);