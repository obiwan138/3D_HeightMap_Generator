# ECE 4122/6122 CUSTOM FINAL PROJECT - Random Height Map Generation and Rendering

**Contributors** : Matthew Luyten, Lydia Jameson, Thomas Ethève

## Project description

This project creates a SFML window with the SFML OpenGL interface and display a random generated height map. The Height map is generated randomly using one of the 4 random generators provided (Fractal, Turbulent, Opalescent, Gradient Weighting). The map is displayed in 3D and can also be visualized in 2D.

The user is free to move in the world using the following keys
- Move Mouse or Trackpad         : Controls the direction
- Press (w,s,a,d)                : Moves (forwad, backward, on the left, on the right) in the plan y = cst 
- Press (Left shift, space bar)  : Moves (down, up)
- Press (Right arrow, Left arrow): (Increases / Decreases) the user speed
- Press R                        : Resets position to (0,0) and looks toward the -z axis (up un 2D map)

The user can also change some view perception with the following keys
- Press (Up arrow / Down arrow)  : (Increases / Decreases) the camera focal angle (unzoom / zoom)
- Press F                        : Toggles the "Fill Triangle fragments" flag. The heightmap can be seen as "textured" or in a wire mesh mode
- Press O                        : Changes the projection mode in orthographic
- Press P                        : Changes the projection mode in perspective
- Press V                        : Toggles the view mode in 2D/3D

The 2D view mode shows the map in a "cartographic" view. The user is still free to move using the keys in this mode, and can locate itself as well as the origin by the pink circle and the red square. The view mode also shows the borders of each chunk.

When the user move towards the border of a chunk, new chunks are created in the moving direction whereas the chunks too far behind are deleted to free memory space (they are not recovered if the user go back to the previous location).

## Compiling instructions

```
mkdir build
cd build
cmake ..
cmake --build . -j8
```

## Building tests

```
mkdir build
cd build
cmake .. -DBUILD_TESTS=on
cmake --build . -j8
```

## Run the main program with the command line arguments

The ```main.exe``` file is located in ```${PROJECT_REPOSITORY}/src```. The main program accepts several command line arguments:

```
#|  ARGUMENTS        |      DEFAULT VALUES   |  DESCRIPTION
# --help, -h,               ---                 print help message
# --size, -s,               100                 set N, the width of each chunk. Each chunk will be size NxN"
# --resolution -r,          0.25                set resolution of the window
# --visibility, -v,         1                   set visibility of the window
# --width, -x               1280                set width of the window
# --height, -y              720                 set height of the window
# --octaves, -o,            8                   set number of octaves for fractal perlin noise
# --seed,                   -random-            set 64bit seed for perlin noise
# --freq-start,             0.05                set starting frequency for fractal perlin noise
# --freq-rate,              2                   set frequency rate for fractal perlin noise
# --amp-rate,               0.5                 set amplitude decay rate for fractal perlin noise
# --mode ,                  0                   Noise mode (0 - fractal, 1 - turbulent, 2 - opalescent, 3 - gradient weighting)
# --max,                    5                   Noise max value
# --cmap, -c,               1                   set color map (0 - GRAY_SCALE, 1 - GIST_EARTH)

# Example of launch command:
./main --size 50 --resolution 0.25 --visibility 2 --width 1280 --height 760 --octaves 8 --freq-start 0.05 --freq-rate 2 --amp-rate 0.5 --mode 0 --max 7 --cmap 1
```
The user is free to use ```run_program_linux.sh``` and   ```run_program_windows.ps1```  to launch the program using the full command line options. For windows user, make sure that you authorize powershell to launch powershell scripts (See ```run_program_windows.ps1```).

## Other details

Notes:
   - Tests requires Boost and gnuplot-iostream libraries. gnuplot-iostream should be in external. If it is not, run `git submodule update --init`.

INSTALLING BOOST
   - Please install boost from their pre-compiled binaries at https://sourceforge.net/projects/boost/files/boost-binaries/
   - NOTE: Please download and install the binaries that correspond to your version of mvcc. If you installed the VisualStudio tookit this calendar year, you likely need the binaries named boost_x_xx_x-msvc-14.3-64.exe - the 14.3 part is the most important!!

BOOST SETUP
   - Boost FOR SOME REASON now requires cmake to find BoostConfig.cmake (why??). The only environment variable you must set is Boost_DIR=/path/to/boost/libXX-msvc-14.X/cmake/Boost-X.XX.X
   - NOTE: For me (mluyten), this path is C:\local\boost_1_86_0\lib64-msvc-14.3\cmake\Boost-1.86.0

ET VOILA, YOU ARE NOW A BOOST USING, RAMBLING GAMBLING, HELLUVA ENGINEER!
