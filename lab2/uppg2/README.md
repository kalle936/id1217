Quicksort implementation using OpenMP

Run with make (run). Either supply make with NWORKERS and SIZE,
otherwise it will use default values which is NWORKERS = 10
and SIZE = 1000000.

Define DEBUG for a debug prints. DO NOT USE with large SIZE!

Can also be run through OMP_NUM_THREADS=NWORKERS ./quicksort.out SIZE.

Uses OpenMP tasks which is implemented in OpenMP 3.0. GCC implements
OpenMP 3.0 in versions 4.4 and later.

Program creates one task that is run by only one thread (otherwise the array
would be sorted several times). Two tasks new tasks are created in every iteration
of quicksort (one for the elements < pivot and one for the elements >= pivot).
The program switches to insertionsort if the interval to be sorted is < a breakpoint
that can be set by the macro BREAKPOINT. The breakpoint used in the evaluation is 50.

~~~ Evaluation ~~~

Using a CPU with 8 hardware threads @ 2GHz because atlantis does not support
OpenMP 3.0 (gcc version is 4.1)

Results were achieved by runing the program five times and taking the median time.
Size of array is 20 000 000 (this is because the sequential execution time
should be at least 1-2 seconds).

Execution time
1 thread    =   2.83s
2 threads   =   1.52s
4 threads   =   0.89s
8 threads   =   0.70s

Speedup
1 thread    =   1 (per definition)
2 threads   =   1.86
4 threads   =   3.18
8 threads   =   4.04

Efficiency
1 thread    =   1 (per definition)
2 threads   =   0.93
4 threads   =   0.795
8 threads   =   0.505

The times is not optimal at all, but this is not to be expected as there is overhead
when creating threads and the first call to quicksort can only be executed sequentially.
This is inherited behaviour from the algorithm quicksort. One way to prevent this could be
to partially sort the array NWORKERS number of times. i.e. to split the array in equal parts
and then merge them. However, the merging needs to be done sequentially so there is not much,
if anything, to be gained from this.

The efficiency as can be seen in the efficiency table decreases rapidly when additional threads
are used. For even more threads than 8 I think the trend would continue. I could not test this
as Atlantis use gcc version 4.1 and thus does not support OpenMP 3.0 which is when tasks were
implemented.
