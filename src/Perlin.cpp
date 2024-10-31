#include "Perlin.hpp"
#include <stddef.h>
#include <cmath>


void perlin1D(std::vector<double> &surface, std::vector<double> &t, std::vector<double> gradient, int resolution) {
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