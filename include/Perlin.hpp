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
 * @author Matt Luyten
 * @brief This is the gradient noise class. It implements both 1D and 2D perlin gradient noise, as well as some of the 
 * gradient noise "tricks" to create more interesting outputs. For repeatable results, this class should
 * be initialized with a seed; however, if no seed is provided, it will seed itself randomly. This class is
 * necessary so that the noise generator can keep track of the gradient vector's it has already generated. This
 * speeds up generation and introduces some flexibility into the way 'randomness' is implemented.
 */ 
class GradientNoise {
public:
    /**
     * @author Matt Luyten
     * @brief GradientNoise default constructor. Initializes random gradient generator with "random" seed based
     * on system time
     */
    GradientNoise();
    /**
     * @author Matt Luyten
     * @brief GradientNoise constructor with user-defined seed
     * 
     * @param seed a 64-bit random seed for noise generator
     */
    GradientNoise(uint32_t seed);
    ~GradientNoise();

    /** 
     * @author Matt Luyten
     * @brief Generates perlin noise (y) for an input (x)
     * 
     * @param x the input x position for noise value y
     * 
     * @return the noise value
     */ 
    double perlin1D(double x);

    /**
     * @author Matt Luyten
     * @brief Generates noise (z) for an input (x, y) + the gradient vector
     * 
     * @param x the input x position for noise value z
     * @param y the input y position for noise value z
     * 
     * @return the noise value
     */
    glm::vec3 perlin2D(double x, double y);

    /**
     * @author Matt Luyten
     * @brief Implements fractal perlin noise in 3 modes (regular, turbulent, and opalescent) for 1D perlin noise
     * Note: this mostly used for testing.
     * 
     * @param x the input x position for noise value y
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
     * @author Matt Luyten
     * @brief Implements fractal perlin noise in 4 modes (regular, turbulent, opalescent, gradient weighting) for 2D perlin noise
     * 
     * @param x the input x position for noise value z
     * @param y y the input y position for noise value z
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
     * @author Matt Luyten
     * @brief Same as above, but accepts a pass-by-reference vec3 for better memory management
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
     * @author Matt Luyten
     * @brief Keeps track of the 2D gradients in this noise generator.
     */
    class Gradient2 {
    public:
        Gradient2(uint32_t seed);
        glm::vec2 at(glm::vec2 pos); // Returns gradient vector at integer position (x,y)
        glm::vec2 at(int x, int y); // Returns gradient vector at integer position (x,y)
    
    private:
        glm::vec2 generate(int x, int y);
        std::map<std::pair<int, int>, glm::vec2> _gradients;
        std::mutex _m;
        uint32_t _seed;
    };

    /**
     * @author Matt Luyten
     * @brief Keeps track of the 1D gradients in this noise generator.
     */
    class Gradient1 {
    public:
        /**
         * @author Matt Luyten
         * @brief 1D gradient constructor
         */
        Gradient1(uint32_t seed);

        /**
         * @author Matt Luyten
         * @brief Gets 1D gradient at integer position x.
         * 
         * @param x the integer position of the requested gradient
         * 
         * @return the requested gradient
         */
        double at(int x);
    private:
        /**
         * @author Matt Luyten
         * @brief Generates a random 1D gradient.
         * 
         * @return a random gradient
         */
        double generate(int x);
        std::map<int, double> _gradients; // Map of gradients so that gradients only have to be generated once
        std::mutex _m; // Semaphore to protect _gradients map
        uint32_t _seed;
    };

    Gradient1 _gradient1; // 2D gradient manager
    Gradient2 _gradient2; // 1D gradient manager
};

uint32_t lfsr(uint32_t seed, size_t shifts);