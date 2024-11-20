#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH --mem=1240M                #I want to use 124Go / node

#SBATCH --qos=debug
#SBATCH --partition=debug
#SBATCH --ntasks-per-node=20

echo "running fast_cpu_conv"
./fast_cpu_conv_eval.x
echo "running cpu_conv"
./cpu_conv_eval.x
echo "running cudnn_conv"
./cudnn_conv_eval.x
echo "running opencl_conv"
./opencl_conv_eval.x
echo "running pytorch mkl-dnn"
python conv2d.py cpu
echo "running pytorch cudnn (should be simular to cudnn_conv)"
python conv2d.py cuda
