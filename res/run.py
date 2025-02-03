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
    pup = clean(filter(data, ".cnf.prj.pup$"))
    pupp = clean(filter(data, ".cnf.prj.pupp$"))

    data = cnf
    data = data.join(prj, on = 'file', rsuffix = '_prj')
    data = data.join(prjp, on = 'file', rsuffix = '_prjp')

    data = data.join(pup, on = 'file', rsuffix = '_pup')
    data = data.join(pupp, on = 'file', rsuffix = '_pupp')

    print(f"{title} ({len(data)}):")

    x = data
    x = x[x['state'] == 'done']
    print(f"   cnf: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prj'] == 'done']
    x = x[x['state_pup'] == 'done']
    print(f"   prj + pup: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prj'] == 'done']
    x = x[x['state_pupp'] == 'done']
    print(f"   prj + pupp: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prjp'] == 'done']
    x = x[x['state_pupp'] == 'done']
    print(f"   prjp + pupp: {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prjp'] == 'done']
    f1 = (x['state_pup'] == 'done') | (x['state_pupp'] == 'done')
    x = x[f1.to_numpy()]
    print(f"   prjp + (pupp | pup): {len(x)} ({len(x) / len(data)})")

    x = data
    x = x[x['state_prjp'] == 'done']
    x = x[x['state_pup'] == 'done']
    m = x.mem_prjp + x.mem_pup
    m /= 1000
    t = x.time_prjp + x.time_pup
    print(f"   prjp + pup: {len(x)} ({len(x) / len(data)})")
    print(f"       4GB: {(m <=  4000).sum()}")
    print(f"       8GB: {(m <=  8000).sum()}")
    print(f"      16GB: {(m <= 16000).sum()}")
    print(f"      32GB: {(m <= 32000).sum()}")
    print(f"       1m : {(t <= 60).sum()}")
    print(f"      10m : {(t <= 600).sum()}")
    print(f"       1h : {(t <= 3600).sum()}")

d4 = pd.read_csv(f"d4.csv", skipinitialspace = True)
summarize(d4, "D4")

pd4 = pd.read_csv(f"pd4.csv", skipinitialspace = True)
summarize(pd4, "PD4")
