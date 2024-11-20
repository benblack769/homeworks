#!/usr/bin/env python3

import subprocess
from compile import compile_file

def file_data(arg_l):
    data_l = []
    for i in range(8,24+1):
        size = 2**i
        iters = (2**28)//size
        if iters > 0:
            out_info = subprocess.check_output(arg_l+[str(size),str(iters)])
            value = float(out_info.decode(encoding='UTF-8').strip())
        else:
            value = 1.0
        data_l.append(value)
    return data_l
        
def data_to_str(data):
    return ','.join([str(d) for d in data])
    
compile_file("up_locs.cpp")
compile_file("asm_up_locs.cpp")
compile_file("vecasm_up_locs.cpp")

data = [
    file_data(["python3","update_locations.py"])
    file_data(["./up_locs"]),
    file_data(["./asm_up_locs"]),
    file_data(["./vecasm_up_locs"]),
    file_data(["python3","npup_locs.py"])
]
for n in range(len(data)):
    gname = "graphdata"+str(n) + ".csv"
    with open(gname,"wb") as gfile:
        for i in range(n+1):
            gfile.write((data_to_str(data[i])+"\n").encode(encoding='UTF-8'))
            
    
    






