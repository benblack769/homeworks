[benblack@c028h177 regular]$  time ./encode compilers.tar compilers.tar.huf

real    0m28.094s
user    0m27.898s
sys     0m0.123s
[benblack@c028h177 regular]$ time ./decode compilers.tar.huf compare.tar

real    0m23.755s
user    0m23.637s
sys     0m0.094s

-rw-rw-r--. 1 benblack benblack 161484800 Feb 17 20:25 compare.tar
-rw-rw-r--. 1 benblack benblack 161484800 Feb 17 20:22 compilers.tar
-rw-rw-r--. 1 benblack benblack 101317080 Feb 17 20:25 compilers.tar.huf
