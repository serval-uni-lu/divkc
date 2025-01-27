import pandas as pd
import numpy as np
import re

def filter(data, ext):
    n = data.file.to_numpy()
    n = map(lambda x : re.search(ext, x) != None, n)
    return data[list(n)]

def clean(data):
    data = data.copy(deep = True)
    data.file = data.file.map(lambda x : x[:x.find('.cnf') + len('.cnf')])
    data.set_index('file', inplace = True)
    return data

def summarize(data, title):
    file_list = set()
    for f in data.file:
        i = f.find(".cnf")
        file_list.add(f[:i + len(".cnf")])

    cnf = clean(filter(data, ".cnf$"))
    prj = clean(filter(data, ".cnf.prj$"))
    prjp = clean(filter(data, ".cnf.prj.p$"))
    s0 = clean(filter(data, ".cnf.s0$"))
    s0p = clean(filter(data, ".cnf.s0.p$"))
    s1 = clean(filter(data, ".cnf.s1$"))
    s1p = clean(filter(data, ".cnf.s1.p$"))

    data = cnf
    data = data.join(prj, on = 'file', rsuffix = '_prj')
    data = data.join(prjp, on = 'file', rsuffix = '_prjp')

    data = data.join(s0, on = 'file', rsuffix = '_s0')
    data = data.join(s0p, on = 'file', rsuffix = '_s0p')

    data = data.join(s1, on = 'file', rsuffix = '_s1')
    data = data.join(s1p, on = 'file', rsuffix = '_s1p')

    print(f"{title} ({len(data)}):")

    x = data
    x = x[x['state'] == 'done']
    print(f"   cnf: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prj'] == 'done']
    x = x[x['state_s0'] == 'done']
    x = x[x['state_s1'] == 'done']
    print(f"   prj + s0 + s1: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prjp'] == 'done']
    x = x[x['state_s0p'] == 'done']
    x = x[x['state_s1p'] == 'done']
    print(f"   prjp + s0p + s1p: {len(x)} ({len(x) / len(data)})")

d4 = pd.read_csv(f"d4.csv", skipinitialspace = True)
summarize(d4, "D4")

pd4 = pd.read_csv(f"pd4.csv", skipinitialspace = True)
summarize(pd4, "PD4")
