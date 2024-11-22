Compiling Instructions

```
mkdir build
cd build
cmake ..
cmake --build . -j8
```

BUILDING TESTS

```
mkdir build
cd build
cmake .. -DBUILD_TESTS=on
cmake --build . -j8
```

Notes:
   - Tests requires Boost and gnuplot-iostream libraries. gnuplot-iostream should be in external. If it is not, run `git submodule update --init`.

INSTALLING BOOST
   - Please install boost from their pre-compiled binaries at https://sourceforge.net/projects/boost/files/boost-binaries/
   - NOTE: Please download and install the binaries that correspond to your version of mvcc. If you installed the VisualStudio tookit this calendar year, you likely need the binaries named boost_x_xx_x-msvc-14.3-64.exe - the 14.3 part is the most important!!

BOOST SETUP
   - Boost FOR SOME REASON now requires cmake to find BoostConfig.cmake (why??). The only environment variable you must set is Boost_DIR=/path/to/boost/libXX-msvc-14.X/cmake/Boost-X.XX.X
   - NOTE: For me (mluyten), this path is C:\local\boost_1_86_0\lib64-msvc-14.3\cmake\Boost-1.86.0

ET VOILA, YOU ARE NOW A BOOST USING, RAMBLING GAMBLING, HELLUVA ENGINEER!
