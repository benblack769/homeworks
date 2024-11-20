CPP=g++
CXX_FLAGS = -mavx -std=c++14 -ffast-math
HALIDEFLAGS = $(CXX_FLAGS) -g -I /install/include/ -L /install/lib/ -fno-rtti  -lHalide -lpng -ljpeg -lpthread -ldl -std=c++11
OPT_FLAGS = -O3
#-ftree-loop-vectorize -fpeel-loops -fsplit-loops -flto
NVCC_PATH = $(shell which nvcc)
CUDA_ROOT = /usr/local/cuda
#$(dir $(NVCC_PATH))/..
CUDA_LIB = $(CUDA_ROOT)/lib64
CUDA_INCLUDE = $(CUDA_ROOT)/include


all: diff_tests perf_evals

diff_tests: cpu_conv_test.x cudnn_conv_test.x opencl_conv_test.x fast_cpu_conv_test.x

perf_evals: cpu_conv_eval.x cudnn_conv_eval.x opencl_conv_eval.x fast_cpu_conv_eval.x

cpu_conv_test.x: cpu_conv_help.o cpu_conv.o gold_conv.o diff_test.o
	$(CPP) -fopenmp cpu_conv_help.o cpu_conv.o gold_conv.o diff_test.o -o $@

cpu_conv_eval.x: cpu_conv_help.o cpu_conv.o perf_test.o
	$(CPP)  -flto -fopenmp cpu_conv_help.o cpu_conv.o perf_test.o -o $@

fast_cpu_conv_test.x: fast_cpu_conv.o gold_conv.o diff_test.o
	$(CPP) -fopenmp fast_cpu_conv.o gold_conv.o diff_test.o -o $@

fast_cpu_conv_eval.x: fast_cpu_conv.o perf_test.o
	$(CPP) -fopenmp fast_cpu_conv.o perf_test.o -o $@

opencl_conv_test.x: opencl_conv.o gold_conv.o diff_test.o
	$(CPP) -L $(CUDA_LIB)   opencl_conv.o gold_conv.o diff_test.o -lOpenCL  -o $@

opencl_conv_eval.x: opencl_conv.o perf_test.o
	$(CPP) -fopenmp -L $(CUDA_LIB)  opencl_conv.o perf_test.o  -lOpenCL -o $@

opencl_conv.o: opencl_impl.cpp conv.h
	$(CPP) $(CXX_FLAGS) $(OPT_FLAGS) -I .  -c  -o $@ $<

halide_conv_test.x: halide.o gold_conv.o diff_test.o
	$(CPP) gold_conv.o diff_test.o  halide.o -pthread conv_generator.a -ldl -o $@

halide_conv_eval.x: halide.o perf_test.o
	$(CPP) perf_test.o  halide.o -pthread -fopenmp conv_generator.a -ldl -o $@

cudnn_conv_test.x: cudnn.o gold_conv.o diff_test.o
	$(CPP) cudnn.o gold_conv.o diff_test.o  -L $(CUDNN_ROOT)/lib64 -L $(CUDA_LIB) -lcudnn  -lcudart -o $@

cudnn_conv_eval.x: cudnn.o perf_test.o
	$(CPP) -fopenmp cudnn.o perf_test.o -L $(CUDNN_ROOT)/lib64 -L $(CUDA_LIB) -lcudnn  -lcudart -o $@

cpu_conv.o: cpu_conv.cpp conv.h
	$(CPP) $(CXX_FLAGS) -fopenmp $(OPT_FLAGS)  -c  -o $@ $<

fast_cpu_conv.o: fast_cpu_conv.cpp conv.h
	$(CPP) $(CXX_FLAGS) -fopenmp $(OPT_FLAGS)  -c  -o $@ $<

cpu_conv_help.o: convolve_help.cpp convolve_help.h
	$(CPP) $(CXX_FLAGS) $(OPT_FLAGS)  -c  -o $@ $<

gold_conv.o: gold_conv.cpp conv.h gold_conv.h
	$(CPP) -g $(CXX_FLAGS) -c  -o $@ $<

diff_test.o: diff_test.cpp conv.h
	$(CPP) $(CXX_FLAGS) -c -o $@ $<

perf_test.o: perf_test.cpp conv.h
	$(CPP) $(CXX_FLAGS) -fopenmp -c -o $@ $<

cudnn.o: cudnn.cpp
	nvcc -c -I $(CUDA_INCLUDE) cudnn.cpp

halide.o: halide.cpp halide_gen.cpp conv.h
	g++ halide_gen.cpp /install/share/tools/GenGen.cpp $(HALIDEFLAGS) -o gen_halide_exec
	LD_LIBRARY_PATH=/install/lib/ ./gen_halide_exec -g conv_generator  -o . target=host
	g++ -I /install/include/ -c halide.cpp

clean:
	rm *.o *.x

test: diff_tests
	echo "running cpu_conv_test"
	./cpu_conv_test.x
	echo "running cuda test"
	./cudnn_conv_test.x
	# echo "running halide_conv_test"
	# ./halide_conv_test.x

perf: perf_evals
	echo "running cpu_conv_eval"
	./cpu_conv_eval.x
