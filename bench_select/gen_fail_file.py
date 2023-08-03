import pandas as pd
import numpy as np

sampler = 'd4'
data =  pd.read_csv(f"{sampler}.csv", skipinitialspace = True, index_col = 'file')

done = data[data['state'] == 'done']
fail = data[data['state'] != 'done']
mem = data[data['state'] == 'mem']
time = data[data['state'] == 'timeout']

# print(fail)
print("max mem on success: " + str(done.mem.max() / 1000000))
print("max time on success: " + str(done.time.max() / 3600))

fail.to_csv(f"{sampler}_fail.csv")
