#!/usr/bin/env python3

import subprocess


def file_data(arg_l):
    data_l = []
    for i in range(8,24+1):
        size = 2**i
        iters = (2**32)//size
        if iters > 0:
            out_info = subprocess.check_output(arg_l+[str(size),str(iters)])
            value = float(out_info.decode(encoding='UTF-8').strip())
        else:
            value = 1.0
        data_l.append(value)
    return data_l
        
def compile_file(cpp_name):
    subprocess.call(["g++6","-O3","-march=native","-mtune=native","-std=c++11","-o",cpp_name[:cpp_name.index('.')],cpp_name])

def data_to_str(data):
    return '\n'.join([str(d) for d in data])
    
compile_file("up_locs.cpp")

data = [
    file_data(["./up_locs"])
]
gname = "cdoublegraph.col"
with open(gname,"wb") as gfile:
    for i,d in enumerate(data[0]):
        gfile.write((str(2**(i+8))+"\t\t\t"+str(d) + "\n").encode(encoding='UTF-8'))
        
    
    






