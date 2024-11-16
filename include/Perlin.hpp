/*
Author: Matthew Luyten
Class: ECE6122
Last Date Modified: 11/16/2024

Description:
This is includes our templated ease curve (interpolation) function and its derivative. This contains the 
declaration for the GradientNoise class.
*/

#pragma once
#include <vector>
#include <map>
#include <ctime>
#include <glm/glm.hpp>
#include <random>
#include <mutex>

/**
 * Quintic interpolation function: f(t) = 6 * t^5 - 15 * t^4 + 10 * t^3 
 * 
 * @param t
 * 
 * @return 
 */
template <typename T>
T easeCurve(T t) { 
    return ((6 * t - 15) * t + 10) * t * t * t;
}

/**
 * Derivative of quintic interpolation function: f'(t) = 30 * t^4 - 60t^3 + 30t^2
 * 
 * @param t
 * 
 * @return
 */
template <typename T>
T easeCurveGradient(T t) {
    return (t * t - 2 * t + 1) * 30 * t * t;
}

/**
 * This is the gradient noise class. It implements both 1D and 2D perlin gradient noise, as well as some of the 
 * gradient noise "tricks" to create more interesting outputs. For repeatable results, this class should
 * be initialized with a seed; however, if no seed is provided, it will seed itself randomly. This class is
 * necessary so that the noise generator can keep track of the gradient vector's it has already generated. This
 * speeds up generation and introduces some flexibility into the way 'randomness' is implemented.
 */ 
class GradientNoise {
public:
    /**
     * GradientNoise default constructor. Initializes random gradient generator with "random" seed based
     * on system time
     */
    GradientNoise();
    /**
     * GradientNoise constructor with user-defined seed
     * 
     * @param seed
     */
    GradientNoise(int64_t seed);
    ~GradientNoise();

    /** 
     * Generates perlin noise (y) for an input (x)
     * 
     * @param x
     * 
     * @return 
     */ 
    double perlin1D(double x);

    /**
     *  Generates noise (z) for an input (x, y) + the gradient vector
     * 
     * @param x
     * @param y
     * 
     * @return
     */
    glm::vec3 perlin2D(double x, double y);

    /**
     * Implements fractal perlin noise in 3 modes (regular, turbulent, and opalescent) for 1D perlin noise
     * Note: this mostly used for testing.
     * 
     * @param x
     * @param octaves
     * @param freqStart
     * @param freqRate
     * @param ampRate
     * 
     * @return
     */
    double fractalPerlin1D(double x, int octaves=8, double freqStart=0.05, 
            double freqRate=2, double ampRate=0.5);

    /**
     * Implements fractal perlin noise in 4 modes (regular, turbulent, opalescent, gradient weighting) for 2D perlin noise
     * 
     * @param x
     * @param y
     * @param max
     * @param octaves
     * @param freqStart
     * @param freqRate
     * @param ampRate
     * 
     * @return
     */
    double fractalPerlin2D(double x, double y, double max=1, int mode=0, int octaves=8, double freqStart=0.025, 
            double freqRate=2, double ampRate=0.5);
    
    /**
     * Same as above, but accepts a pass-by-reference vec3 for better memory management
     * 
     * @param pos
     * @param max
     * @param octaves
     * @param freqStart
     * @param freqRate
     * @param ampRate
     * 
     * @return
     */
    void fractalPerlin2D(glm::vec3& pos, double max=1, int mode=0, int octaves=8, double freqStart=0.025, 
            double freqRate=2, double ampRate=0.5);
private:
    /**
     * Keeps track of the 2D gradients in this noise generator.
     */
    class Gradient2 {
    public:
        Gradient2(int64_t seed);
        glm::vec2 at(glm::vec2 pos); // Returns gradient vector at integer position (x,y)
        glm::vec2 at(int x, int y); // Returns gradient vector at integer position (x,y)
    private:
        glm::vec2 generate();
        std::map<std::pair<int, int>, glm::vec2> _gradients;
        std::mutex _m;
    };

    /**
     * Keeps track of the 1D gradients in this noise generator.
     */
    class Gradient1 {
    public:
        Gradient1(int64_t seed);
        double at(int x);
    private:
        double generate();
        std::map<int, double> _gradients;
        std::mutex _m;
    };

    
    Gradient1 _gradient1; // 2D gradient manager
    Gradient2 _gradient2; // 1D gradient manager
};