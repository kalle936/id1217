Compute Sum, min and max of matrix elements.

Computes the sum, min and max of elements in a matrix in parallel using
OpenMP.

Compile through make rule matrixSum-openmp.out "make matrixSum-openmp.out".
Run through default make rule run. This will compile if needed and then run
with specified size and number of workers. Size can be specified through SIZE
and number of workers through NWORKERS.

Example usage = "NWORKERS=4 SIZE=10000 make".

The parallel part in the program is a for loop where each iteration computes
a single row in the matrix. The scheduling should be done static because the
workload in each iteration is the same. This makes it better for static than
dynamic.

Inside the for loop every column is iterated over and the sum is updated
accordingly. Also for every column in the row the element is checked if it
is lesser than min and if it is greater than max - if so update min/max.

The updating of min and max is done in a critical section to avoid any race
condition.

I changed the matrix implementation from the original code structure to become
a 2d vector on the heap. Indexing in the matrix is now done through
matrix[i * size + j]. This is slower than otherwise but since it is equally
slow for every excecution the timings should not be scewed by it.

Evaluation.

Using a CPU with 8 hardware threads @ 2GHz on a 30 000 x 30 000 matrix.

1 thread    =   1.901s
2 threads   =   0.977s
4 threads   =   0.524s
8 threads   =   0.535s

Speedup
1 thread    =   1
2 threads   =   1.95
4 threads   =   3.63
8 threads   =   3.55

Efficiancy
1 thread    =   1
2 threads   =   0.975
4 threads   =   0.908
8 threads   =   0.444

We can see that the speed up and efficiancy is very good when utilizing 2 or 4
cores and then plumits really bad when utilizing 8 cores. There are different
possible reasons for this. One is that false sharing becomes a much bigger factor
when 8 threads are splitting the workload among them. Another one could be that
the threads wait for the critical section in a much larger percentage than with
2 och 4 threads.
