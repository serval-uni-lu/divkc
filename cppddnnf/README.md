# Projection

## Dependencies

On Ubuntu and Debian-based systems:
```
sudo apt install ninja-build libboost-dev libgmp-dev libboost-random-dev libboost-program-options-dev g++
```

## Building

The `build.ninja` file is generated with:
```
g++ gen.cpp -o gen
./gen
```
or if the `build.ninja` file already exists:
```
ninja generate
```

To build the executable please run:
```
ninja
```

This will create multiple executables, namely:
```
build/appmc
build/lmc
build/pc
build/rhsampler
build/rsampler
build/sampler
```

In the paper we only used `build/appmc` for the approximate model counter
and `build/sampelr` for the sampling algorithm.

To only build those you can use
```
ninja build/appmc build/sampler
```

## Usage

### Appmc

The executable used for approximate model counting is `build/appmc`.
Runnin `build/appmc --help` gives:
```
Allowed options:
  -h [ --help ]         Display help message
  --cnf arg             path to CNF file
  --alpha arg (=0.01)   alpha value for the CLT
  --nb arg (=10000)     the maximum number of samples to use
```

The `cnf` argument is the path to the CNF file.
The `alpha` parameter is the parameter used by the central limit theorem to compute
the confidence interval and `nb` is the maximum number of samples to use before the algorithm
stops.

This executable assumes that if the `cnf` parameter is set to `t.cnf`, there will be
files called `t.cnf.log`, `t.cnf.pnnf` for the projected formula (`t.cnf.p`) compiled to d-DNNF
and `t.cnf.unnf` for the upper bound (`t.cnf.pup`) compiled to d-DNNF.
These files can be generated with the help of the version of `D4` shipped in this repository
and the `projection` executable.

### Sampler

The executable used for sampling is `build/sampler`.
Runnin `build/sampler --help` gives:
```
Allowed options:
  -h [ --help ]         Display help message
  --cnf arg             path to CNF file
  --k arg (=50)         buffer size to use
  --nb arg (=10)        the number of solutions to generate
```

The `cnf` argument is the path to the CNF file.
The `k` parameter is the buffer size which is to be used.
Larger values mean that the sampler will be close to uniformity.
However, sampling will be more expensive.
The `nb` parameter indicates the desired number of solutions.

This executable assumes that if the `cnf` parameter is set to `t.cnf`, there will be
files called `t.cnf.log`, `t.cnf.pnnf` for the projected formula (`t.cnf.p`) compiled to d-DNNF
and `t.cnf.unnf` for the upper bound (`t.cnf.pup`) compiled to d-DNNF.
These files can be generated with the help of the version of `D4` shipped in this repository
and the `projection` executable.

### RSampler

`build/rsampler` is similar to `build/sampler` but it uses reservoir sampling (and is not
presented in the paper).
We therefore do not present theorems for the bounds of uniformity of `rsampler`.
However, `rsampler` is much more sampler efficient than `sampler` and can therefore be
used with much larger buffer sizes than `sampler`.
To test `sampler` we had to use buffer sizes of 50 to execute the statistical unifomity
tests in reasonable time. To test `rsampler` we used buffer sizes of 50000 instead.

The usage and requirements of `rsampler` are the same as for `sampler`.

Here are the raw test results for `build/sampler` as executed for the paper:
(The `+` symbols indicate which tests the sampler passed.)

|File                                     |  #F |     HMP | #Fails | #Bonferroni | Time (h) |
|-----------------------------------------|-----|---------|--------|-------------|----------|
|omega_birthday_b1000_c10_ksampler.csv    | 140 | 0.005   |      2 |           1 |   40.611 |
|omega_birthday_b2000_c10_ksampler.csv    | 137 | 0.001   |      1 |           1 |   27.037 |
|omega_birthday_b4000_c10_ksampler.csv    | 142 | 0.112 + |      1 |           0 |   44.457 |
|omega_chisquared_b1000_c10_ksampler.csv  |  65 | 0.000   |     11 |           9 |   40.851 |
|omega_chisquared_b2000_c10_ksampler.csv  |  63 | 0.000   |     10 |           9 |   34.417 |
|omega_chisquared_b4000_c10_ksampler.csv  |  60 | 0.000   |     10 |           7 |   36.711 |
|omega_freq_nb_var_b1000_c10_ksampler.csv |  77 | 0.000   |     21 |          18 |   49.026 |
|omega_freq_nb_var_b2000_c10_ksampler.csv |  77 | 0.000   |     20 |          15 |   49.074 |
|omega_freq_nb_var_b4000_c10_ksampler.csv |  77 | 0.000   |     19 |          18 |   49.414 |
|omega_freq_var_b1000_c10_ksampler.csv    | 176 | 0.000   |     65 |          54 |   26.862 |
|omega_freq_var_b2000_c10_ksampler.csv    | 175 | 0.000   |     72 |          58 |   25.638 |
|omega_freq_var_b4000_c10_ksampler.csv    | 175 | 0.000   |     82 |          63 |   26.739 |
|omega_modbit_q128_b1000_c10_ksampler.csv |  83 | 0.000   |     25 |          19 |   60.208 |
|omega_modbit_q128_b2000_c10_ksampler.csv |  83 | 0.000   |     23 |          20 |   59.110 |
|omega_modbit_q128_b4000_c10_ksampler.csv |  83 | 0.000   |     24 |          19 |   59.044 |
|omega_modbit_q16_b1000_c10_ksampler.csv  | 178 | 0.000   |      9 |           3 |    3.291 |
|omega_modbit_q16_b2000_c10_ksampler.csv  | 177 | 0.000   |     13 |           7 |    1.457 |
|omega_modbit_q16_b4000_c10_ksampler.csv  | 177 | 0.000   |     11 |           9 |    2.881 |
|omega_modbit_q2_b1000_c10_ksampler.csv   | 178 | 0.199 + |      0 |           0 |    3.124 |
|omega_modbit_q2_b2000_c10_ksampler.csv   | 178 | 0.047 + |      3 |           0 |    5.764 |
|omega_modbit_q2_b4000_c10_ksampler.csv   | 177 | 0.138 + |      2 |           0 |    3.196 |
|omega_modbit_q32_b1000_c10_ksampler.csv  | 178 | 0.000   |     28 |          16 |    8.699 |
|omega_modbit_q32_b2000_c10_ksampler.csv  | 177 | 0.000   |     26 |          17 |    7.328 |
|omega_modbit_q32_b4000_c10_ksampler.csv  | 177 | 0.000   |     27 |          19 |    8.244 |
|omega_modbit_q4_b1000_c10_ksampler.csv   | 178 | 0.114 + |      3 |           0 |    3.565 |
|omega_modbit_q4_b2000_c10_ksampler.csv   | 177 | 0.151 + |      2 |           0 |    1.506 |
|omega_modbit_q4_b4000_c10_ksampler.csv   | 177 | 0.081 + |      1 |           0 |    3.219 |
|omega_modbit_q64_b1000_c10_ksampler.csv  | 126 | 0.000   |     40 |          27 |   63.311 |
|omega_modbit_q64_b2000_c10_ksampler.csv  | 125 | 0.000   |     36 |          28 |   58.559 |
|omega_modbit_q64_b4000_c10_ksampler.csv  | 125 | 0.000   |     34 |          31 |   59.194 |
|omega_modbit_q8_b1000_c10_ksampler.csv   | 178 | 0.129 + |      1 |           0 |    3.205 |
|omega_modbit_q8_b2000_c10_ksampler.csv   | 178 | 0.017 + |      1 |           0 |    6.432 |
|omega_modbit_q8_b4000_c10_ksampler.csv   | 177 | 0.189 + |      1 |           0 |    3.048 |


Here are the raw test results for `build/rsampler`:

|File                                     |  #F |     HMP | #Fails | #Bonferroni | Time (h) |
|-----------------------------------------|-----|---------|--------|-------------|----------|
|omega_birthday_b1000_c10_rsampler.csv    | 139 | 0.000   |      4 |           1 |   23.282 |
|omega_birthday_b2000_c10_rsampler.csv    | 141 | 0.000   |      2 |           2 |   17.580 |
|omega_birthday_b4000_c10_rsampler.csv    | 142 | 0.000   |      2 |           2 |   30.186 |
|omega_chisquared_b1000_c10_rsampler.csv  |  70 | 0.185 + |      0 |           0 |   25.985 |
|omega_chisquared_b2000_c10_rsampler.csv  |  65 | 0.201 + |      0 |           0 |   17.663 |
|omega_chisquared_b4000_c10_rsampler.csv  |  66 | 0.048 + |      2 |           0 |   28.888 |
|omega_freq_nb_var_b1000_c10_rsampler.csv |  82 | 0.196 + |      1 |           0 |   34.427 |
|omega_freq_nb_var_b2000_c10_rsampler.csv |  82 | 0.197 + |      1 |           0 |   33.029 |
|omega_freq_nb_var_b4000_c10_rsampler.csv |  83 | 0.092 + |      4 |           0 |   37.798 |
|omega_freq_var_b1000_c10_rsampler.csv    | 176 | 0.000   |      6 |           1 |   12.896 |
|omega_freq_var_b2000_c10_rsampler.csv    | 176 | 0.000   |      8 |           4 |   11.597 |
|omega_freq_var_b4000_c10_rsampler.csv    | 176 | 0.000   |      8 |           2 |   11.462 |
|omega_modbit_q128_b1000_c10_rsampler.csv |  88 | 0.000   |      1 |           1 |   49.047 |
|omega_modbit_q128_b2000_c10_rsampler.csv |  88 | 0.000   |      3 |           2 |   43.098 |
|omega_modbit_q128_b4000_c10_rsampler.csv |  89 | 0.000   |      2 |           1 |   45.706 |
|omega_modbit_q16_b1000_c10_rsampler.csv  | 178 | 0.206 + |      0 |           0 |    0.947 |
|omega_modbit_q16_b2000_c10_rsampler.csv  | 178 | 0.172 + |      2 |           0 |    1.415 |
|omega_modbit_q16_b4000_c10_rsampler.csv  | 178 | 0.040 + |      2 |           0 |    2.565 |
|omega_modbit_q2_b1000_c10_rsampler.csv   | 178 | 0.127 + |      3 |           0 |    0.842 |
|omega_modbit_q2_b2000_c10_rsampler.csv   | 178 | 0.118 + |      4 |           0 |    1.411 |
|omega_modbit_q2_b4000_c10_rsampler.csv   | 178 | 0.069 + |      3 |           0 |    2.560 |
|omega_modbit_q32_b1000_c10_rsampler.csv  | 178 | 0.277 + |      0 |           0 |    4.598 |
|omega_modbit_q32_b2000_c10_rsampler.csv  | 178 | 0.139 + |      2 |           0 |    5.178 |
|omega_modbit_q32_b4000_c10_rsampler.csv  | 178 | 0.000   |      4 |           2 |    6.000 |
|omega_modbit_q4_b1000_c10_rsampler.csv   | 178 | 0.124 + |      3 |           0 |    0.877 |
|omega_modbit_q4_b2000_c10_rsampler.csv   | 178 | 0.203 + |      0 |           0 |    1.510 |
|omega_modbit_q4_b4000_c10_rsampler.csv   | 178 | 0.120 + |      3 |           0 |    2.598 |
|omega_modbit_q64_b1000_c10_rsampler.csv  | 132 | 0.170 + |      1 |           0 |   44.348 |
|omega_modbit_q64_b2000_c10_rsampler.csv  | 132 | 0.054 + |      2 |           0 |   42.039 |
|omega_modbit_q64_b4000_c10_rsampler.csv  | 132 | 0.000   |      2 |           1 |   41.509 |
|omega_modbit_q8_b1000_c10_rsampler.csv   | 178 | 0.141 + |      2 |           0 |    0.919 |
|omega_modbit_q8_b2000_c10_rsampler.csv   | 178 | 0.091 + |      3 |           0 |    1.387 |
|omega_modbit_q8_b4000_c10_rsampler.csv   | 178 | 0.138 + |      1 |           0 |    2.584 |

We can definetly see that `rsampler` passes more tests. Moreover,
for the Modbit test with `q = 8` we see that `sampler` needed 3 hours while `rsampler`
only needed 1 hour.

## Apptainer

It is also possible to use Apptainer (Singularity) instead of using a native builds.
Simply run
```
apptainer build --fakeroot appmc.sif appmc.def
apptainer build --fakeroot sampler.sif sampler.def
apptainer build --fakeroot rsampler.sif rsampler.def
```
to create the containers.
You may then use the containers just like the native executable presented above.
