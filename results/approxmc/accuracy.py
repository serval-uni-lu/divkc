import pandas as pd
import numpy as np
from mpmath import mp
from statistics import median
from statistics import mean

clt_prefix = "n10000.seps1.1"
approxmc_suffix = "e0.8d0.1"
epsilon = 1.2

mp.pretty = True

def safe_mpf(x):
    try:
        if pd.notna(x):
            return mp.mpf(x)
        else:
            np.nan
    except Exception:
        return None  # or mp.mpf('0.0'), etc.

def keep_done(data):
    return data[data['state'] == 'done']

def filter_on_index(data, s):
    return data[data.index.str.contains(s)]


total = pd.read_csv("csv/total.csv", skipinitialspace = True)


mc       = pd.read_csv("csv/mc.csv", skipinitialspace = True, index_col = 'file')
approxmc = pd.read_csv(f"csv/approxmc_{approxmc_suffix}.csv", skipinitialspace = True, index_col = 'file')
clt      = pd.read_csv(f"csv/{clt_prefix}/clt.csv", skipinitialspace = True, index_col = 'file')
pnnf     = pd.read_csv(f"csv/{clt_prefix}/pnnf.csv", skipinitialspace = True, index_col = 'file')
unnf     = pd.read_csv(f"csv/{clt_prefix}/unnf.csv", skipinitialspace = True, index_col = 'file')
split    = pd.read_csv(f"csv/{clt_prefix}/split.csv", skipinitialspace = True, index_col = 'file')
proj     = pd.read_csv(f"csv/{clt_prefix}/proj.csv", skipinitialspace = True, index_col = 'file')

mc['mc'] = mc['mc'].apply(safe_mpf)
approxmc['amc'] = approxmc['amc'].apply(safe_mpf)
clt['Y'] = clt['Y'].apply(safe_mpf)
clt['Yl'] = clt['Yl'].apply(safe_mpf)
clt['Yh'] = clt['Yh'].apply(safe_mpf)

mc.dropna(inplace = True)
approxmc.dropna(inplace = True)
clt.dropna(inplace = True)
pnnf.dropna(inplace = True)
unnf.dropna(inplace = True)
split.dropna(inplace = True)
proj.dropna(inplace = True)

approxmc = keep_done(approxmc)
clt = keep_done(clt)
pnnf = keep_done(pnnf)
unnf = keep_done(unnf)
split = keep_done(split)
proj = keep_done(proj)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lappmc = mc.join(approxmc, on = 'file')
    lclt = mc.join(clt, on = 'file')

    lappmc.dropna(inplace = True)
    lclt.dropna(inplace = True)

    lappmc = filter_on_index(lappmc, sub)
    lclt = filter_on_index(lclt, sub)

    lclt_a = (lclt['Y'] <= (lclt['mc'] * epsilon)) & (lclt['Y'] >= (lclt['mc'] / epsilon))
    lapp_a = (lappmc['amc'] <= (lappmc['mc'] * epsilon)) & (lappmc['amc'] >= (lappmc['mc'] / epsilon))

    app_ra = '-'
    clt_ra = '-'

    if len(lapp_a) != 0:
        app_ra = f"{sum(lapp_a) / len(lapp_a):5.3f}"
    if len(lclt_a) != 0:
        clt_ra = f"{sum(lclt_a) / len(lclt_a):5.3f}"

    print(f"{vsub} & {len(lapp_a)} & {app_ra} & {len(lclt_a)} & {clt_ra} \\\\")
