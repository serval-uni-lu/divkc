# Reproduction

This folder contains the scripts necessary to generate Table
5 (`accuraccy.py` or `accuraccy_md.py`) and Table 6 (`runtime.py`
or `runtime_md.py`) of our paper.

## Dependencies

The python dependencies for the scripts can be installed with the following command:
`pip install pandas numpy mpmath`

## Data

The `csv` folder contains multiple files, which we describe here.

The `mc.csv` file contains the true model count of the formulae for which
we managed to run `D4` within 64GB of memory and 5 hours.
The model counts for a formula `t.cnf` can be computed with
(within a 64GB and 5 hours limit imposed with a call to `/divkc/wrap 64000 18000`,
memory expressed in MB and time expressed in seconds respectively):
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
    /divkc/wrap 64000 18000 /divkc/d4 -mc "t.cnf" 2>&1 \
    | grep -E "^s " | sed 's/^s //g'
```


The `total.csv` file contains a summary of the datasets.
The first column is the actual folder containing the formulae.
The second column is the number of formulae in the folder
and the last column (`map`) is the text to be used for the
LaTeX table generation.

The results for approxmc are stored in files called
`approxmc_e{epsilon}d{delta}.run.csv` where the `epsilon` and `delta`
values are parameters given to approxmc.
The first column is the file name.
The second column (`amc`) is the approximate model count returned by
approxmc. The third column (`state`), fourth column (`mem`) and fith
column (`time`) represent the state of the program, the maximum amount
of memory used in kB and the execution time in seconds.
The `state`, `mem` and `time` columnd can be obtained by wrapping the call
to `approxmc` with our wrapper located in `D4/wrapper`.

As an example, we ran approxmc with a limit of 64GB of memory and 5 hours
a value of `0.2` for `delta` and `0.1` for `epsilon`:
```
wrap 64000 18000 /path/to/approxmc -d 0.2 -e 0.1 -s "$RANDOM" "t.cnf"
```
For our usual example we get the following output:
```
...
s SATISFIABLE
s mc 26208

./approxmc -d 0.2 -e 0.1 -s 27266 cnf/plazar/FeatureModels/FM-3.6.1-refined.cnf, done, 20877, 0.10312
```
The python scripts expect the computation time to be the total time required to
do 4 runs of approxmc. That sum is then multiplied by 5 to simulate 20 runs.

Now we follow with the files in the `n{#samples}.seps{epsilon}` and
`n{#samples}.ns`.
The folder `n{#samples}.seps{epsilon}` designates a run of our approximate model counter
with at most `#samples` and early stopping enabled with the `epsilon` parameter set
to `epsilon`.
The folder `n{#samples}.ns` is similar but early stopping is disabled and the algorithm
will use the all `#samples` samples.

Each `n{#samples}.seps{epsilon}` folder contains multiple files:
```
clt.run.csv
pnnf.csv
proj.csv
split.csv
unnf.csv
```

The `clt.run.csv` file contains the data related to the run of the approximate model
counting algorithm described in our paper.
The file contains the `N, Y, Yl, Yh` columns printed by our approximate counting algorithm
which represents the number of sampels used, the estimated model count, the lower bound
for the confidence interval, and the upper bound for the confidence interval respectively.
The last columns are `state, mem, time` which are obtained by wrapping the call
to `appmc` with our wrapper as shown above for approx mc.

The contents for `pnnf.csv` and `unnf.csv` can be generated with our
`docker_compile_formula.sh` script as follows:
```
bash docker_compile_formula.sh t.sh
```
prints:
```
...
          file, state, mem, time
pnnf.csv: t.cnf, done, 579751, 0.0174753

          file, state, mem, time
unnf.csv: t.cnf.unnf, done, 584146, 0.0163802
```

The contents for `split.csv` and `proj.csv` are also printed by the
`docker_compile_formula.sh` script:
```
...
           file, state, mem, time
split.csv: t.cnf, done, 7540, 0.00225897

          file, state, mem, time
proj.csv: t.cnf.proj, done, 4, 0.00236412
```

The `clt.run.csv` file contains the result and runtime of the `appmc` procedure
descriped in pour paper.
Similarily to `approxmc`, the scripts expect the runtimes to be the sum
of four runs which get multiplied by five by the `runtime.py` script to
simulate 20 runs. Since compilation only need to be done once even for
multiple calls to an approximate model counter, only the call to `appmc` needs to be repeated.
The data to populate `clt.run.csv` is obtained by running
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
    /divkc/wrap 64000 18000 \
    /divkc/appmc --epsilon 1.1 --nb 10000 --cnf t.cnf
```
Which prints:
```
N,Y,Yl,Yh
10000, 26379.6, 25804.5, 26954.7

/divkc/appmc --epsilon 1.1 --nb 10000 --cnf t.cnf, done, 536424, 0.0198274
```

### Helper script

The `divkc/results/gen.sh` script can be used to generate some of the files.
For a folder `tmp` containing DIMACS `.cnf` files, running
```
bash gen.sh tmp
```
produces the following files:

- tmp.bounds.csv
- tmp.cls.csv
- tmp.clt.csv
- tmp.clt.run.csv

  Please note that early stopping is enabled in the `gen.sh` script
  Also, the approximate model counting procedure is run 4 times.
  The execution times are summed together and for the memory, we use the maximum.
  For the estimates, we only keep the ones from the last run.
- tmp.mc.csv
- tmp.pnnf.csv
- tmp.proj.csv
- tmp.split.csv
- tmp.unnf.csv

To generate the tables from this subfolder, the following files are necessary:

- tmp.clt.run.csv
- tmp.mc.csv
- tmp.pnnf.csv
- tmp.proj.csv
- tmp.split.csv
- tmp.unnf.csv

The following files are necessary but not generated by `gen.sh`:

- total.csv
- approxmc_....run.csv

Please not that in this example, the files are names `tmp.{}.csv` but
the names are hardcoded in the python scripts as `n10000.seps1.1/{}.csv`.
Moreover, the lines in `approxmc_....run.csv` are expected to represent
4 runs of approxmc. The 4 runs are aggregated by taking the maximum memory
consumption and computing the sum of the execution times. The approximate
model count can be any of the returned values.
