#pragma once
#include <vector>
#include <map>
#include <ctime>
#include <glm/glm.hpp>
#include <random>

template <typename T>
T easeCurve(T t) { // f(t) = 6 * t^6 - 15 * t^5 + 10 * t^3 
    return ((6 * t - 15) * t + 10) * t * t * t;
}

class GradientNoise {
public:
    GradientNoise();
    GradientNoise(int64_t seed);
    ~GradientNoise();

    double perlin1D(double x);
    double perlin2D(double x, double y);
    void perlin2D(glm::vec3& pos);

    double fractalPerlin1D(double x, int octaves=8, double freqStart=0.05, 
            double freqRate=0.5, double ampRate=0.5);
    double fractalPerlin2D(double x, double y, int octaves=8, double freqStart=0.05, 
            double freqRate=0.5, double ampRate=0.5);
    void fractalPerlin2D(glm::vec3& pos, int octaves=8, double freqStart=0.05, 
            double freqRate=0.5, double ampRate=0.5);

private:
    class Gradient2 {
    public:
        Gradient2(int64_t seed);
        glm::vec2 at(glm::vec2);
        glm::vec2 at(int x, int y);
    private:
        glm::vec2 generate();
        std::map<std::pair<int, int>, glm::vec2> _gradients;
    };

    class Gradient1 {
    public:
        Gradient1(int64_t seed);
        double at(int x);
    private:
        double generate();
        std::map<int, double> _gradients;
    };

    Gradient1 _gradient1;
    Gradient2 _gradient2;
};