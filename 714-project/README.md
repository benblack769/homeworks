### Project


### Installation

To compile the code on a deepthought2 node, run:

```
module load cuda
module load cudnn
module load pytorch
make -j8
```

### Run

To do performance tests, run

```
sbatch run_eval_slurm.sh
```

To run the performance test on a GPU node, run

```
sbatch run_eval_gpu.sh
```

### Test

To do accuracy tests vs the gold convolution algorithm, run

```
make test
```

### Program structure

`conv.h` contains the declaration for the grouped convolution operation. This operation is implemented by the following files:

* cpu_conv.cpp: image first cpu convolution implementation
* fast_cpu_conv.cpp: mat_mul cpu convolution implementation
* opencl_impl.cpp: Opencl implementation for GPU
* cudnn.cpp: A cudnn implementation (not included in paper writeup)

The convolve operation defined in these files is separately linked to the following programs:

* gold_conv.cpp: a simple implementation to test the other implementations against for accuracy (not for performance).
* diff_test.cpp: Compares `convolve` function to the gold convolve algorithm and checks that the results are similar
* perf_test.cpp: Computes `convolve` on various inputs and gets the FLOPs of the algorithm
