#!/usr/local/bin/python

import sys
import subprocess

def compile_file(cpp_name):
    subprocess.call(["g++","-O3","-march=native","-mtune=native","-std=c++11","-o",cpp_name[:cpp_name.index('.')],cpp_name])

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("required arguments: filename")
        exit(1)
    else:
        compile_file(sys.argv[1])
        