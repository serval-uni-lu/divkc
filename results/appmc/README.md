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

The `cnf.clt.csv` file
The `cnf.lmc.csv` file
The `cnf.mc.csv` file
The `total.csv` file
