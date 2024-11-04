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
