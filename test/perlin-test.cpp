
#include "Perlin.hpp"
#include "gnuplot-iostream.h"
#include <tuple>

int main() {
    Gnuplot gp;

    int ngrad=10;
    int dimensions = 1;
    double resolution=32;
    /*
    std::vector<double> surf(10000, 0);
    std::vector<double> t(10000, 0);

    for (int i = 0; i < 10000; i++) {
        double freq = 0.0005;
        double amplitude = 1;
        for (int j = 0; j < 8; j++) {
            surf[i] += amplitude * perlin1D((i-5000)*freq, 0xDEADBEEF);
            amplitude *= 0.5;
            freq *= 2;
        }
        t[i] = i;
    }

    auto plots = gp.plotGroup();
    plots.add_plot1d(std::tuple(t, surf), "with lines title '1d perlin surface'");
    gp << plots;
    */
    size_t N = 500;
    std::vector<std::vector<double>> data(N, std::vector<double>(N, 0));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double freq = 0.01;
            double amplitude = 1;
            for (int k = 0; k < 8; k++) {
                data[i][j] += amplitude * perlin2D((i - 250.0)*freq, (j - 250.0)*freq, 0xDEADBEEF);
                amplitude *= 0.5;
                freq *= 2;
            }
        }
    }

    // Send data to gnuplot
    gp << "set pm3d map\n";
    gp << "set palette rgbformulae 33,13,10\n"; // Set a color palette
    gp << "splot '-' matrix with image\n";

    for (const auto &row : data) {
        for (const auto &value : row) {
            gp << value << " ";
        }
        gp << "\n";
    }
    gp << "e\n";
}