# Evaluation of our URS procedure

## Contents

- The `csv` folder contains the results of the evaluation of our procedure
  on the `omega` dataset proposed in [TURS](https://doi.org/10.1145/3797477)

- The `summary.py` python script can be used to generate a markdown table of
  which summarizes the contents of the raw csv files.

## Usage

By running:
```
python summary.py csv/*.csv
```

We obtain:

|File                                         |  #F |     HMP | #Fails | #Bonferroni | Time (h)|
|-                                            |   - |       - |      - |           - |        -|
|csv/omega_birthday_b1000_c10_ksampler.csv    | 140 | 0.005   |      2 |           1 |   40.611|
|csv/omega_birthday_b1000_c10_rsampler.csv    | 139 | 0.000   |      4 |           1 |   23.282|
|csv/omega_chisquared_b1000_c10_ksampler.csv  |  65 | 0.000   |     11 |           9 |   40.851|
|csv/omega_chisquared_b1000_c10_rsampler.csv  |  70 | 0.185 + |      0 |           0 |   25.985|
|csv/omega_freq_nb_var_b1000_c10_ksampler.csv |  77 | 0.000   |     21 |          18 |   49.026|
|csv/omega_freq_nb_var_b1000_c10_rsampler.csv |  82 | 0.196 + |      1 |           0 |   34.427|
|csv/omega_freq_var_b1000_c10_ksampler.csv    | 176 | 0.000   |     65 |          54 |   26.862|
|csv/omega_freq_var_b1000_c10_rsampler.csv    | 176 | 0.000   |      6 |           1 |   12.896|
|csv/omega_modbit_q128_b1000_c10_ksampler.csv |  83 | 0.000   |     25 |          19 |   60.208|
|csv/omega_modbit_q128_b1000_c10_rsampler.csv |  88 | 0.000   |      1 |           1 |   49.047|
|csv/omega_modbit_q16_b1000_c10_ksampler.csv  | 178 | 0.000   |      9 |           3 |    3.291|
|csv/omega_modbit_q16_b1000_c10_rsampler.csv  | 178 | 0.206 + |      0 |           0 |    0.947|
|csv/omega_modbit_q2_b1000_c10_ksampler.csv   | 178 | 0.199 + |      0 |           0 |    3.124|
|csv/omega_modbit_q2_b1000_c10_rsampler.csv   | 178 | 0.127 + |      3 |           0 |    0.842|
|csv/omega_modbit_q32_b1000_c10_ksampler.csv  | 178 | 0.000   |     28 |          16 |    8.699|
|csv/omega_modbit_q32_b1000_c10_rsampler.csv  | 178 | 0.277 + |      0 |           0 |    4.598|
|csv/omega_modbit_q4_b1000_c10_ksampler.csv   | 178 | 0.114 + |      3 |           0 |    3.565|
|csv/omega_modbit_q4_b1000_c10_rsampler.csv   | 178 | 0.124 + |      3 |           0 |    0.877|
|csv/omega_modbit_q64_b1000_c10_ksampler.csv  | 126 | 0.000   |     40 |          27 |   63.311|
|csv/omega_modbit_q64_b1000_c10_rsampler.csv  | 132 | 0.170 + |      1 |           0 |   44.348|
|csv/omega_modbit_q8_b1000_c10_ksampler.csv   | 178 | 0.129 + |      1 |           0 |    3.205|
|csv/omega_modbit_q8_b1000_c10_rsampler.csv   | 178 | 0.141 + |      2 |           0 |    0.919|

# References

- TURS: Zeyen, Olivier, et al. "Testing Uniform Random Samplers: Methods, Datasets and Protocols." ACM Transactions on Software Engineering and Methodology (2025).
  
  [GitHub](https://github.com/serval-uni-lu/urs_test)
