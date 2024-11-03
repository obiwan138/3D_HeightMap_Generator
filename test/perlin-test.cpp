
#include "Perlin.hpp"
#include "gnuplot-iostream.h"
#include <boost/program_options.hpp>
#include <tuple>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    bool perlin1D = false;
    std::srand(time(NULL));
    po::variables_map vm;  
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "print help")
            ("1d", "plot Noise in one dimension")
            ("size,s", po::value<size_t>()->default_value(100), "set N number of points to calculate (NxN for 2D)")
            ("octaves,o", po::value<int>()->default_value(8), "set number of octaves for fractal perlin noise")
            ("seed", po::value<int64_t>()->default_value(std::rand()), "set 64bit seed for perlin noise")
            ("freq-start", po::value<double>()->default_value(0.05), "set starting frequency for fractal perlin noise")
            ("freq-rate", po::value<double>()->default_value(2), "set frequency rate for fractal perlin noise")
            ("amp-rate", po::value<double>()->default_value(0.5), "set amplitude decay rate for fractal perlin noise")
        ;
              
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        perlin1D = vm.count("1d");
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    Gnuplot gp;
    size_t size = vm["size"].as<size_t>();

    if (perlin1D) {
        std::vector<double> data(size, 0);
        std::vector<double> t(size, 0);
        for (int i = 0; i < size; i++) {
            data[i] = fractalPerlin1D(i, vm["seed"].as<int64_t>(), vm["octaves"].as<int>(),
                    vm["freq-start"].as<double>(), vm["freq-rate"].as<double>(), vm["amp-rate"].as<double>());
            t[i] = i;
        }
        
        auto plots = gp.plotGroup();
        plots.add_plot1d(std::tuple(t, data), "with lines title '1d perlin surface'");
        gp << plots;
    }
    else {
        std::vector<std::vector<double>> data(size, std::vector<double>(size, 0));

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                data[i][j] += fractalPerlin2D(i, j, vm["seed"].as<int64_t>(), vm["octaves"].as<int>(),
                        vm["freq-start"].as<double>(), vm["freq-rate"].as<double>(), vm["amp-rate"].as<double>());
            }
        }

        // Send data to gnuplot
        gp << "set pm3d map\n";
        gp << "set palette gray\n"; // Set a color palette
        gp << "splot '-' matrix with image\n";

        for (const auto &row : data) {
            for (const auto &value : row) {
                gp << value << " ";
            }
            gp << "\n";
        }
        gp << "e\n";
    }
}