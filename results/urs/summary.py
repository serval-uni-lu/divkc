import pandas as pd
import numpy as np
import math
import sys

significance_level = 0.01

def format_float(x):
    return f"{x:.3f}"

def format_pvalue(x):
    if x <= significance_level:
        return format_float(x) + "  "
    else:
        return format_float(x) + " +"

def left_padder(width):
    return lambda x : ((width - len(x)) * ' ') + x

def right_padder(width):
    return lambda x : x + ((width - len(x)) * ' ')

def pad_left(l):
    return list(map(left_padder(max(map(len, l))), l))

def pad_right(l):
    return list(map(right_padder(max(map(len, l))), l))

def add_header(l, h):
    l.insert(0, h)

files   = []
nbfs    = []
pvals   = []
nfails  = []
nbfails = []
ctimes  = []

for i in sys.argv[1:]:
    data = pd.read_csv(i, skipinitialspace = True, index_col = 'file')
    data.dropna(inplace = True)

    files.append(i)
    nb = len(data)
    nbfs.append(nb)

    pr = 1.0 / ((1.0 / nb) * (np.sum(1.0 / data['pvalue'])))
    pvals.append(pr)

    nb_rejects = len(data[data.pvalue <= (significance_level)])
    nfails.append(nb_rejects)

    nb_rejects = len(data[data.pvalue <= (significance_level / nb)])
    nbfails.append(nb_rejects)

    ctimes.append(np.sum(data['time']) / 3600)

nbfs = list(map(str, nbfs))
pvals = list(map(format_pvalue, pvals))
nfails = list(map(str, nfails))
nbfails = list(map(str, nbfails))
ctimes = list(map(format_float, ctimes))

add_header(files, '-')
add_header(nbfs, '-')
add_header(pvals, '-')
add_header(nfails, '-')
add_header(nbfails, '-')
add_header(ctimes, '-')

add_header(files, 'File')
add_header(nbfs, '#F')
add_header(pvals, 'HMP')
add_header(nfails, '#Fails')
add_header(nbfails, '#Bonferroni')
add_header(ctimes, 'Time (h)')

files = pad_right(files)
nbfs = pad_left(nbfs)
pvals = pad_left(pvals)
nfails = pad_left(nfails)
nbfails = pad_left(nbfails)
ctimes = pad_left(ctimes)

for l in zip(files, nbfs, pvals, nfails, nbfails, ctimes):
    print('|' + ' | '.join(l) + '|')

