# Reproduction

This folder contains the scripts necessary to generate Tables 1 to 4 of our paper.
The `csv` folder contains the raw data and the `gen_tables.py` can be used to
generate the LaTeX tables.
The `gen_tables_md.py` script can be used to generate the markdown versions of the
tables.

## Data

The `csv` folder contains multiple files, which we describe here.

The `cnf.bound.csv` file contains the lower and upper bounds to the model
count of the input formula. The `low` column is the number of models
printed by `D4` when compiling `t.cnf.proj.p` to `t.cnf.pnnf` (`G_P` in
the paper). The `high` column is the number of models
printed by `D4` when compiling `t.cnf.proj.pup` to `t.cnf.unnf` (`G_U` in
paper).


The `cnf.cls.csv` file contains how many variables (`#v`) and how many clauses (`#c`)
each formula has.

The `docker_compile_formula.sh` script can be used to generate the above
tables.
When running `bash docker_compile_formula.sh t.cnf`, the script outputs
```
               file,low,high
cnf.bound.csv: t.cnf, 2, 54000

             file,#v,#c
cnf.cls.csv: t.cnf, 45, 104
```

The `cnf.clt.csv` file contains the output of the approximate model counting procedure.
When running `docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc /divkc/appmc --cnf t.cnf`
we get
```
N,Y,Yl,Yh
10000, 26686.9, 25748.2, 27625.6
```

The `cnf.lmc.csv` file contains the output of the approximate lower bound computed
with the `lmc` executable. This file is not used for the table generation and
the `lmc` executable is not included in the docker image.
This file is unnecessary.

The `cnf.mc.csv` file contains the true model count of the formulae for which
we managed to run `D4` within 64GB of memory and 5 hours.
The model counts for a formula `t.cnf` can be computed with
(within a 64GB and 5 hours limit imposed with a call to `/divkc/wrap 64000 18000`,
memory expressed in MB and time expressed in seconds respectively):
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" divkc \
    /divkc/wrap 64000 18000 /divkc/d4 -mc "t.cnf"
```


The `total.csv` file contains a summary of the datasets.
The first column is the actual folder containing the formulae.
The second column is the number of formulae in the folder
and the last column (`map`) is the text to be used for the
LaTeX table generation.
