Systems homework. 

Files:
	Benchmark source files:
		update_locations.py 		python benchmark
		np_up_locs.py				numpy benchmark (for fun)
		up_locs.cpp					ordinary c++ benchmark
		asm_up_locs.cpp				inline assembly benchmark
		vecasm_up_locs.cpp			AVX fully vectorised inline assembly benchmark
	
	Helper files:
		run_tests.py 				general purpose compiler and runner of c and python tests
		c-double-test.py 			helper for getting output for c function
		translate.py 				helper for translating csv lists to collumns for gnuplot
		cfldograph.txt 				produces the plot for the c float vs double calculation
		graphplot.txt				produces plot for python/c/asm/vectorized asm
		
	Data files:
		.col files stand for the collum format that gnuplot likes
		.png files store the plots
		
	Compile options for c:
		g++ -O3 -march=native -mtune=native -std=c++11 -o <source>  <source>.cpp
		using gcc version 6 on ubuntu, mingw64 version 5.2 on Windows
		
		python <source>.py
		using cpython version 3.5 on windows

To generate the plot, run "run_tests.py" which outputs csv files, then run "translate.py" which generates .col files which are then read when you run gnuplot on "graphplot.txt"

To get ordinary output like checksums from any of the files, uncomment the lines which do that, and comment out the line that only outputs the average time.


Analysis:

1 a:
	I picked iterations = 2^27//size for testing python code because it was running really slowly on more iterations and I did not want to wait all day, but I did want at least around 10 iterations on the largest size.
	
1.b:
	I ran all the sizes at once for all versions and kept them if different sizes seemed to have reasonably consistent performance with each other. I figured that if they had consistent performance with each other it was unlikely that there were other factors affecting performance.
	
1.c:
	Plot is in graph.png
	
	
2.	For the c and assembly versions, I picked iterations = 2^30//size for testing because I was getting too much variation with the 2^27//size, and I could run higher numbers of iterations much faster. This greatly improved the consistency of the tests. I also switched to an Ubuntu boot on the same machine because Windows thought that it was a great time to run backgound tasks while I was waiting for my benchmarks to finish, which was ruining results. 

3. Plot is in c_cmp_graph.png. Double precision calculations were only slightly slower than single precision if the data small (presumably in the cach), but almost 2x slower when data was large (presumably not in the cach).

4. Assembly data is in the original plot (uses single precision floating point values). It has nearly identical performance with the c version.

5. To measure memory consumption, I simply executed the code on 2^20 size and 1000 iterations and paused it before it dealocated memory. Then I simply looked at the memory useage in the Windows task manager and wrote it down. I then ran it on a size 1 and saw how much memory it took to do anything at all and subtracted this number from the first. This was to see how much memory it took to actually run the computations, rather than just constant overhead. Here are the results:

	cpython	-> 191.5 MB
	pypy 	-> 82.7 MB
	c 		-> 24.2 MB
	asm 	-> 24.2 MB

As expected, python is hugely inefficient, and c and assembly are identical as they both have identical code which handles memory allocations and dealocations. I think it is kind of cool that pypy not only executes nearly as fast as c, but it also uses less memory than python, so I put that there.

6. This is much better than the previous best for single precision floats. It runs around 5 faster than the c or assembly version on small sizes and slightly better on huge sizes (around 30% faster).
