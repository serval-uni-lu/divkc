# DivKC: A Divide-and-Conquer Approach to Knowledge Compilation

This repository contains the programs and data associated with
the article 'DivKC: A Divide-and-Conquer Approach to Knowledge Compilation'.

## Results

The `results` contains the raw results that are presented in the article as well
as more detailed versions of the tables available in the article.

## D4

The folder `D4` contains a slightly modified version of the D4 compiler.
The modification is only present to also include the unconstrained
varables in the generated .nnf file therefore simplifying further
usage of the d-DNNF.

## Usage

### Splitter

The folder `splitter` contains the used splitting heuristic.
It is compiled by simply typing `make -j8`.
A singularity script is also provided in the folder.

To use the splitter simply type:
`splitter <CNF file> <number of partitions> > <CNF file>.log`

### Projection

The folder `projection` contains the program used for CNF projection.
It is compiled with `make -j8`.
A singularity script is also provided in the folder.

An example usage with a formula named `t.cnf`:

### Approximate algorithms

The folder `cppddnnf` contains the C++ utilities necessary to work with our compiled form.
Installing the dependencies on ubuntu is done with the commmand
```
    sudo apt install ninja-build make g++ libboost-dev libgmp-dev libboost-random-dev
```
The project is then compiled with
```
    g++ gen.cpp -o gen
    ./gen
    ninja
```
This will generate the `build/appmc` and the `build/sampler` files.

`appmc` can be used to perform approximate model counting:
```
splitter t.cnf 4 > t.cnf.log
cat t.cnf.log t.cnf > t.cnf.proj
projection t.cnf.proj
d4 -dDNNF "t.cnf.proj.p" -out="t.cnf.pnnf"
d4 -dDNNF "t.cnf.proj.pup" -out="t.cnf.unnf"

appmc "t.cnf" 1000 0.01
```
Here we run our approximate model counter with 1000 samples
and an alpha value of 0.01.

`sampler` can be used to perform random sampling:
```
splitter t.cnf 4 > t.cnf.log
cat t.cnf.log t.cnf > t.cnf.proj
projection t.cnf.proj
d4 -dDNNF "t.cnf.proj.p" -out="t.cnf.pnnf"
d4 -dDNNF "t.cnf.proj.pup" -out="t.cnf.unnf"

sampler "t.cnf" 1000 50
```
Here we run our random sampler to generate 1000 samples by using
a buffer size of 50.
