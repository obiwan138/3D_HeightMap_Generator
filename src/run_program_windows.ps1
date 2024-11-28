################################################################
#       RUN THE PROGRAM WITH SELECTED ARGUMENTS ON WINDOWS     #
################################################################

# COMMAND TO RUN THIS FILE
# 1 - Once - First you should see authorize the powershell script to run (ExecutionPolicy value is RemoteSigned)
#   1.1 Launch windows powershell in administrator mode and run the following commands:
#   1.2 Get-ExecutionPolicy
#   1.3 Set-ExecutionPolicy RemoteSigned
# 2 - Run the following command in the terminal to run the script 
# powershell ".\run_program_windows.ps1"

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

# Launch the program
./main --size 50 --resolution 0.25 --visibility 2 --width 1280 --height 760 --octaves 8 --freq-start 0.05 --freq-rate 2 --amp-rate 0.5 --mode 0 --max 7 --cmap 1