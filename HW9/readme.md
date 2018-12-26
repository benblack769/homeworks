Note that all files except converter.cpp and conv2.cpp are valid c code, I just named them cpp files to be consistent.

converter.cpp and conv2.cpp needs to be compiled with g++ -std=c++11.

Reported times are on a Skylake processor, running at around 3.3ghz.

conv1.cpp is the trivial version that runs in around 0.0055 seconds, around 6 times faster than the one that calls the library (0.03 seconds). Maybe this has something to do with glibc's implementation
 being 500 lines of code, and being quite general, allowing different bases, character types, signs, and output lengths.

convunroll.cpp takes advantage of the fact that 32 bit integers can only store 10 digits. The optimizer then can unroll this loop completely, and do some more clever optimizations on the results. It runs in about 0.00444 seconds on average.

conv2.cpp is a slight variation of convunroll. It takes about 0.00440 seconds. It tries to exploit the superscalar nature of the cpu more than convunroll.cpp, by computing two numbers together. Unfortunately, it does not make it much faster, only about 1% on my machine, which is hard to measure due to things like varying clock speeds. Looking at the generated assembly output of convunroll, it is clear why this doesn't work very well, it is already doing a very good job of leveraging the CPU. In fact, running perf stat, the convunroll code is evaluating 3.45 instructions per second, which I understand to be close to optimal. It is also slightly higher than the reported instruction throughput than conv2.cpp which is about 3.32. So something is making conv2 faster, just not what I thought. This code also runs much slower (around 20%) than convunroll on the math lab machines, even using the same optimizer (I just moved the generated assembly code over to those machines and recompiled). So right now this is the fastest version, but it is really not much better than convunroll at this point, and may be substantially worse on different architectures.

converter.cpp works by first rearranging the data so that many numbers can be computed at once using vector AVX2 instructions.

I spent many hours working on converter.cpp, and I got it to be faster than the trivial version on some occasions on my machine. Unfortunately, it never got faster than conv2.cpp no matter how many specializations I made for it, and how many shortcuts and abuses of the interface I tried. Through the dozens of code iterations I put it through, it stayed around 0.0055 and 0.006 seconds on my machine.

I think this is because it spends most of the time simply looking at the string byte by byte and copying the data into the temporary buffer, and the so the parallel arithmetic instructions did not help. For some reason, there are also around 3 times the number of branch misses, according to perf stat, so this may help account for the slowness. This is all especially sad because the AVX2 instruction set is one of the reasons why I upgraded my old computer.
