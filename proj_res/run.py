import pandas as pd

base = pd.read_csv(f"base.csv", skipinitialspace = True, index_col = 'file')
prj = pd.read_csv(f"prj.csv", skipinitialspace = True, index_col = 'file')
rem = pd.read_csv(f"rem.csv", skipinitialspace = True, index_col = 'file')
s0 = pd.read_csv(f"s0.csv", skipinitialspace = True, index_col = 'file')
s1 = pd.read_csv(f"s1.csv", skipinitialspace = True, index_col = 'file')
split0 = pd.read_csv(f"split0.csv", skipinitialspace = True, index_col = 'file')
split1 = pd.read_csv(f"split1.csv", skipinitialspace = True, index_col = 'file')
up = pd.read_csv(f"up.csv", skipinitialspace = True, index_col = 'file')


data = base
data = data.join(prj, on = 'file', rsuffix = '_prj')
data = data.join(rem, on = 'file', rsuffix = '_rem')
data = data.join(s0, on = 'file', rsuffix = '_s0')
data = data.join(s1, on = 'file', rsuffix = '_s1')
data = data.join(split0, on = 'file', rsuffix = '_split0')
data = data.join(split1, on = 'file', rsuffix = '_split1')
data = data.join(up, on = 'file', rsuffix = '_up')

# data.dropna(inplace = True)
# print(data)

print(f"total count: {len(data)}")

x = data
x = x[x['state_prj'] == 'done']
x = x[x['state_split0'] == 'done']
x = x[x['state_split1'] == 'done']
print(f"prj + split0 + split1: {len(x)} ({len(x) / len(data)})")

x = data
x = x[x['state_prj'] == 'done']
x = x[x['state_up'] == 'done']
print(f"prj + up: {len(x)} ({len(x) / len(data)})")

x = data
x = x[x['state_prj'] == 'done']
x = x[x['state_s0'] == 'done']
x = x[x['state_s1'] == 'done']
print(f"prj + s0 + s1: {len(x)} ({len(x) / len(data)})")

