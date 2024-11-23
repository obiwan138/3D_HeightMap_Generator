#########################################################
#       RUN THE PROGRAM WITH SELECTED ARGUMENTS         #
#########################################################

#|  ARGUMENTS        |      DEFAULT VALUES   |  DESCRIPTION
# --help, -h,               ---                 print help message
# --size, -s,               100                 set N, the width of each chunk. Each chunk will be size NxN"
# --octaves, -o,            8                   set number of octaves for fractal perlin noise
# --seed,                   -random-            set 64bit seed for perlin noise
# --freq-start,             0.05                set starting frequency for fractal perlin noise
# --freq-rate,              2                   set frequency rate for fractal perlin noise
# --amp-rate,               0.5                 set amplitude decay rate for fractal perlin noise
# --mode ,                  0                   Noise mode (0 - fractal, 1 - turbulent, 2 - opalescent, 3 - gradient weighting)
# --max,                    5                   Noise max value

# Launch the program
./main --size 100 --octaves 8 --freq-start 0.05 --freq-rate 2 --amp-rate 0.5 --mode 0 -x --max 5