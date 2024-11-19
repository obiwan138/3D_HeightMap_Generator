/*
Author: Matthew Luyten
Class: ECE6122
Last Date Modified: 11/16/2024

Description: 
*/

#define _USE_MATH_DEFINES

#include "Perlin.hpp"
#include <stddef.h>
#include <cmath>

/**
 * @author Matt Luyten
 * @brief This implements a 32-bit LFSR that generates a pseudorandom sequence with a uniform distribution.
 * 
 * @param seed the initial state of the LFSR
 * @param shifts the number of times to shift the LFSR
 * 
 * @returns the final state of the LFSR after being shifted. This should be a pseudorandom number.
 */
uint32_t lfsr(uint32_t seed, size_t shifts) {
    for (size_t i = 0; i < shifts; i++) {
        seed |= seed == 0;   // if seed == 0, set seed = 1 instead
        seed ^= (seed & 0x0007ffff) << 13;
        seed ^= seed >> 17;
        seed ^= (seed & 0x07ffffff) << 5;
        seed = seed & 0xffffffff;
    }
    return seed;
}

/** 
 * @author Matt Luyten
 * @brief GradientNoise default constructor. Initializes random gradient generator with "random" seed based
 * on system time
 */
GradientNoise::GradientNoise() : _gradient1(std::time(NULL)), _gradient2(std::time(NULL)) {}

/**
 * @author Matt Luyten
 * @brief GradientNoise constructor with user-defined seed
 * 
 * @param seed a 64-bit random seed for noise generator
 */
GradientNoise::GradientNoise(uint32_t seed) : _gradient1(seed), _gradient2(seed) {}

/**
 * @author Matt Luyten
 * @brief GradientNoise destructor
 */
GradientNoise::~GradientNoise() {}

/**
 * @author Matt Luyten
 * @brief Gradient2 constructor with specified seed
 * 
 * @param seed the desired seed
 */
GradientNoise::Gradient2::Gradient2(uint32_t seed) {
    _seed = seed;
}

/**
 * @author Matt Luyten
 * @brief Gets gradient at integer position (x, y)
 * 
 * @param pos two dimensional vector of gradient position
 * 
 * @return a random gradient
 */
glm::vec2 GradientNoise::Gradient2::at(glm::vec2 position) {
    std::pair<int, int> pos = std::pair<int, int>(position.x, position.y);
    std::unique_lock<std::mutex> lock(_m); // Protect _gradients when emplacing/reading
    if (_gradients.count(pos) == 0) { // Check if gradient at this position
        _gradients.emplace(pos, generate(position.x, position.y)); // If no gradient, generate one
    }
    return _gradients[pos];
}

/**
 * @author Matt Luyten
 * @brief Gets gradient at integer position (x, y)
 * 
 * @param x the x position of the desired gradient
 * @param y the y position of the desired gradient
 * 
 * @return the gradient at (x, y)
 */
glm::vec2 GradientNoise::Gradient2::at(int x, int y) {
    std::pair<int, int> pos = std::pair<int, int>(x, y);
    std::unique_lock<std::mutex> lock(_m); // Protect _gradients when emplacing/reading
    if (_gradients.count(pos) == 0) { // Check if gradient at this position
        _gradients.emplace(pos, generate(x, y)); // If no gradient, generate one
    }
    return _gradients[pos];
}

/**
 * @author Matt Luyten
 * @brief Generates a random 2D gradient.
 * 
 * @param x the y position of the desired gradient
 * @param y the y position of the desired gradient
 * 
 * @return a random gradient
 */
glm::vec2 GradientNoise::Gradient2::generate(int x, int y) {
    int N = 8; // Number of gradients available
    uint32_t perm = lfsr(_seed + x, abs(x)); // Get a random seed based on x
    perm = lfsr(perm + y, abs(y)); // Get a random number using previous seed and y
    perm = perm % N; // Coerce random number to [0,7]

    // Return corresponding gradient
    if (perm == 0)
        return glm::vec2(1, 0);
    else if (perm == 1)
        return glm::vec2(-1, 0);
    else if (perm == 2)
        return glm::vec2(0, 1);
    else if (perm == 3)
        return glm::vec2(0, -1);
    else if (perm == 4)
        return glm::vec2(0.7071, 0.7071);
    else if (perm == 5)
        return glm::vec2(0.7071, -0.7071);
    else if (perm == 6)
        return glm::vec2(-0.7071, 0.7071);
    else
        return glm::vec2(-0.7071, -0.7071);
}

/**
 * @author Matt Luyten
 * @brief 1D gradient constructor
 */
GradientNoise::Gradient1::Gradient1(uint32_t seed) {
    _seed = seed;
}

/**
 * @author Matt Luyten
 * @brief Gets 1D gradient at integer position x.
 * 
 * @param x the integer position of the requested gradient
 * 
 * @return the requested gradient
 */
double GradientNoise::Gradient1::at(int x) {
    std::unique_lock<std::mutex> lock(_m); // Protect _gradients when emplacing/reading
    if (_gradients.count(x) == 0) { // Check if gradient at this position
        _gradients.emplace(x, generate(x)); // If no gradient, generate one
    }
    return _gradients[x];
}

/**
 * @author Matt Luyten
 * @brief Generates a random 1D gradient.
 * 
 * @param x position of the desired gradient
 * 
 * @return a random gradient
 */
double GradientNoise::Gradient1::generate(int x) {
    int N = 5;
    uint32_t perm = lfsr(_seed + x, abs(x)); // Get random number based on value of x
    perm = perm % N; // Coerce random number to [0, 5]

    // Return corresponding gradient
    if (perm == 0)
        return 1;
    else if (perm == 1)
        return 0.5;
    else if (perm == 2)
        return 0;
    else if (perm == 3)
        return -0.5;
    else
        return -1;
}

/**
 * @author Matt Luyten
 * @brief Generates noise (z) for an input (x, y) + the gradient vector
 * 
 * @param x the input x position for noise value z
 * @param y the input y position for noise value z
 * 
 * @return the noise value
 */
double GradientNoise::perlin1D(double x) {
    double p = x;
    double u = p - floor(p); // Get non-integer component of x
    double n0 = _gradient1.at(floor(p)) * u; // Get vector from gradient0 to x
    double n1 = _gradient1.at(ceil(p)) * (u-1); // Get vector from gradient1 to x
    double fade = easeCurve<double>(u); // Interpolate u along ease curve
    return n0 + fade * (n1-n0); // Get continuous interpolated value between gradients
}

/**
 * @author Matt Luyten
 * @brief Generates noise (z) for an input (x, y) + the gradient vector
 * 
 * @param x the input x position for noise value z
 * @param y the input y position for noise value z
 * 
 * @return the noise value
 */
glm::vec3 GradientNoise::perlin2D(double x, double y) {
    double u = x - floor(x); // Get fractional component of x
    double v = y - floor(y); // Get fractional component of y
    double n00 = glm::dot(_gradient2.at(floor(x), floor(y)), glm::vec2(u, v)); // Get vector from gradient00 to pos
    double n10 = glm::dot(_gradient2.at(floor(x)+1, floor(y)), glm::vec2(u-1, v)); // Get vector from gradient10 to pos
    double n01 = glm::dot(_gradient2.at(floor(x), floor(y)+1), glm::vec2(u, v-1)); // Get vector from gradient01 to pos
    double n11 = glm::dot(_gradient2.at(floor(x)+1, floor(y)+1), glm::vec2(u-1, v-1)); // Get vector from gradient11 to pos
    double nx0 = n00 * (1 - easeCurve<double>(u)) + n10 * easeCurve<double>(u); // Interpolate between gradient00 and gradient10
    double nx1 = n01 * (1 - easeCurve<double>(u)) + n11 * easeCurve<double>(u); // Interpolate between gradient 01 and gradient 11
    return glm::vec3(easeCurveGradient<double>(u) * (n10 - n00 + (n00 - n10 - n01 + n11) * easeCurve<double>(v)), // Get derivative in X direction
            easeCurveGradient<double>(v) * (n00 - n01 + (n00 - n10 - n01 + n11) * easeCurve<double>(u)), // Get derivative in Y direction
            nx0 * (1 - easeCurve<double>(v)) + nx1 * easeCurve<double>(v)); // Interpolate y component between 
}

/**
 * @author Matt Luyten
 * @brief Implements fractal perlin noise in 3 modes (regular, turbulent, and opalescent) for 1D perlin noise
 * Note: this mostly used for testing.
 * 
 * @param x the input x position for noise value y
 * @param octaves number of octaves of noise to layer
 * @param freqStart noise frequency starting value. Higher frequency with create more "spiky" heightmaps
 * @param freqRate rate of frequency change between octaves
 * @param ampRate rate of amplitude change between octaves
 * 
 * @return
 */
double GradientNoise::fractalPerlin1D(double x, int octaves, double freqStart, double freqRate, double ampRate) {
    double y = 0;
    double freq = freqStart; // Set starting frequency
    double amplitude = 1; // Set starting amplitude
    for (int k = 0; k < octaves; k++) { // Iterate for k octaves
        y += amplitude * perlin1D(x*freq); // Add perlin noise at amplitude and frequency
        amplitude *= ampRate; // Decrease amplitude
        freq *= freqRate; // Increase frequency
    }
    return y;
}

/**
 * @author Matt Luyten
 * @brief Implements fractal perlin noise in 4 modes (regular, turbulent, opalescent, gradient weighting) for 2D perlin noise
 * 
 * @param x the input x position for noise value z
 * @param y  the input y position for noise value z
 * @param max maximum value (+/-) of the noise
 * @param octaves number of octaves of noise to layer
 * @param freqStart noise frequency starting value. Higher frequency with create more "spiky" heightmaps
 * @param freqRate rate of frequency change between octaves
 * @param ampRate rate of amplitude change between octaves
 * 
 * @return noise value at position (x,y)
 */
double GradientNoise::fractalPerlin2D(double x, double y, double max, int mode, int octaves, double freqStart, double freqRate, double ampRate) {
    double height = 0; // Zero out return value
    double freq = freqStart; // Set starting frequency
    double amplitude = 1; // Set starting amplitude

    // These make all modes make similarly bumpy noise at the same starting frequency
    if (mode == 1 || mode == 2)
        freq = freq / 2;
    
    for (int k = 0; k < octaves; k++) { // Iterate for k octaves
        glm::vec3 noise = perlin2D(x*freq, y*freq); // Get perlin noise at this octave
        // Adjust weighting based on slope at position. Fractal components are weighted less for larger the gradients
        if (k == 1 && mode == 3) amplitude / (sqrt(noise.x * noise.x + noise.y * noise.y) * 2 + 0.8);
        
        if (mode == 1 || mode == 2) // Use magnitude of noise for modes 1 and 2 (turbulent & opalescent)
            height += amplitude * abs(noise.z);
        else // Standard fractal
            height += amplitude * noise.z;

        amplitude *= ampRate; // Decrease amplitude
        freq *= freqRate; // Increase frequency
    }

    // Scale noise value so that it does not exceed max
    if (mode == 1)
        height = height * max * 2 - max;
    else if (mode == 2)
        height = max * cos(2 * M_PI * height);
    else
        height *= max;
    return height;
}

/**
 * @author Matt Luyten
 * @brief Same as above, but accepts a pass-by-reference vec3 for better memory management
 * 
 * @param pos
 * @param max maximum value (+/-) of the noise
 * @param octaves number of octaves of noise to layer
 * @param freqStart noise frequency starting value. Higher frequency with create more "spiky" heightmaps
 * @param freqRate rate of frequency change between octaves
 * @param ampRate rate of amplitude change between octaves
 * 
 * @return noise value at position (x,y)
 */
void GradientNoise::fractalPerlin2D(glm::vec3& pos, double max, int mode, int octaves, double freqStart, double freqRate, double ampRate) {
    double height = 0; // Zero out return value
    double freq = freqStart; // Set starting frequency
    double amplitude = 1; // Set starting amplitude

    // These make all modes make similarly bumpy noise at the same starting frequency
    if (mode == 1 || mode == 2)
        freq = freq / 2;

    for (int k = 0; k < octaves; k++) { // Iterate for k octaves
        glm::vec3 noise = perlin2D(pos.x*freq, pos.z*freq); // Get perlin noise at this octave
        // Adjust weighting based on slope at position. Fractal components are weighted less for larger the gradients
        if (k == 1 && mode == 3) amplitude / (sqrt(noise.x * noise.x + noise.y * noise.y) * 2 + 0.8);
        
        if (mode == 1 || mode == 2)  // Use magnitude of noise for modes 1 and 2 (turbulent & opalescent)
            pos.y += amplitude * abs(noise.z);
        else // Standard fractal
            pos.y += amplitude * noise.z;

        amplitude *= ampRate; // Decrease amplitude
        freq *= freqRate; // Increase frequency
    }

    // Scale noise value so that it does not exceed max
    if (mode == 1)
        pos.y = pos.y * 2 * max - max;
    else if (mode == 2)
        pos.y = max / 5 * cos(2 * M_PI * pos.y);
    else
        pos.y *= max;
    return;
}