import pandas as pd
import numpy as np
from mpmath import mp

mp.pretty = True

mc = pd.read_csv("cnf.mc.csv", skipinitialspace = True, index_col = 'file')
clt = pd.read_csv("cnf.clt.csv", skipinitialspace = True, index_col = 'file')
# glmc = pd.read_csv("cnf.lmc.csv", skipinitialspace = True, index_col = 'file')
bounds = pd.read_csv("cnf.bounds.csv", skipinitialspace = True, index_col = 'file')
total = pd.read_csv("total.csv", skipinitialspace = True)

mc.dropna(inplace = True)
clt.dropna(inplace = True)
# glmc.dropna(inplace = True)
bounds.dropna(inplace = True)
total.dropna(inplace = True)

d = clt.join(mc, on = 'file')
# d = d.join(glmc, on = 'file')
d = d.join(bounds, on = 'file')
d.dropna(inplace = True)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldf = d[d.index.str.contains(sub)]
    lmc = mc[mc.index.str.contains(sub)]
    lgb = clt[clt.index.str.contains(sub)]

    nmc = len(lmc)

    slgb = set(lgb.index)
    slmc = set(lmc.index)

    k = len(slgb - slmc)

    nd4 = len(slmc)
    nt = len(slgb)
    nboth = len(slmc.intersection(slgb))


    if k > 0:
        k = "\\textbf{" + str(k) + "}"
    # print(f"{vsub} & {nbf} & {len(lmc)} & {k} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {ntlow:5.3f} \\\\")
    print(f"{vsub} & {nbf} & {nboth} & {nbf - len(lmc)} & {k} \\\\")

print("------------------------------------------------------------")
for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldf = d[d.index.str.contains(sub)]
    lmc = mc[mc.index.str.contains(sub)]
    lgb = clt[clt.index.str.contains(sub)]

    nmc = len(lmc)

    slgb = set(lgb.index)
    slmc = set(lmc.index)

    k = len(slgb - slmc)

    nlow = 0
    nhigh = 0
    nboth = 0
    nb = 0

    ntlow = 0
    nthigh = 0

    for y in ldf.index:
        yl = mp.mpf(ldf.Yl[y])
        yh = mp.mpf(ldf.Yh[y])
        lo = mp.mpf(ldf.low[y])
        hi = mp.mpf(ldf.high[y])
        tm = mp.mpf(ldf.mc[y])

        ntlow += (tm >= lo)
        nthigh += (tm <= hi)

        nlow += yl <= tm
        nhigh += yh >= tm
        nboth += (yh >= tm) and (yl <= tm)
        nb += 1


    if nb > 0:
        nlow /= nb
        nhigh /= nb
        nboth /= nb

        ntlow /= nb
        nthigh /= nb

        if k > 0:
            k = "\\textbf{" + str(k) + "}"
        # print(f"{vsub} & {nbf} & {len(lmc)} & {k} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {ntlow:5.3f} \\\\")
        print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {ntlow:5.3f} & {nthigh:5.3f} \\\\")
    else:
        print(f"{vsub} & {nb} & & & & & \\\\")

print("------------------------------------------------------------")

# d = clt.join(bounds, on = 'file')
# d.dropna(inplace = True)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldf = d[d.index.str.contains(sub)]
    lmc = mc[mc.index.str.contains(sub)]
    lgb = clt[clt.index.str.contains(sub)]

    nlow = 0
    nhigh = 0
    nboth = 0
    nbothc = 0
    # nboth = 0
    # nlvl = 0
    # ntlow = 0
    nb = 0

    resl = []

    for y in ldf.index:
        yl = mp.mpf(ldf.Yl[y])
        yh = mp.mpf(ldf.Yh[y])
        lo = mp.mpf(ldf.low[y])
        hi = mp.mpf(ldf.high[y])
        tm = mp.mpf(ldf.mc[y])

        nlow += (yl >= lo) and (yl <= tm)
        nhigh += (yh <= hi) and (yh >= tm)
        nboth += (yl >= lo) and (yh <= hi)
        nbothc += (yl >= lo) and (yh <= hi) and (yl <= tm) and (yh >= tm)

        nb += 1
        if yh < yl:
            print(f"    error y: {y} : {yh} > {yl}")
        if hi < lo:
            print(f"    error h: {y} : {hi} > {lo}")

        if (yl >= lo) and (yh <= hi) and (yl <= tm) and (yh >= tm):
            resl.append((yh - yl) / (hi - lo))


    if nb > 0:
        nlow /= nb
        nhigh /= nb
        # ntlow /= nb
        # nlvl /= nb
        # nboth /= nb
        nbothc /= nb

        # print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {nbothc:5.3f} \\\\")
        print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nbothc:5.3f} & {mp.nstr(min(resl), 3)} & {mp.nstr(max(resl), 3)} \\\\")

        # print(f"{vsub} & {nbf} & {len(lmc)} & {k} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {ntlow:5.3f} \\\\")
        # print(f"{vsub} & {nbf} & {len(lmc)} & {k} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} \\\\")
    else:
        # print(f"{vsub} & {nbf} & {len(lmc)} & {k} & & & \\\\")
        print(f"{vsub} & {nb} & & & & & \\\\")
