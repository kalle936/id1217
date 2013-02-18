2. The Bear and Honeybees Problem (Multiple producers - one consumer) (20 points).

Simulates several bees and a bear. The bees gather honey and the bear
eats the honey when the pot of honey is full. 

Compile with make file using "make feedthebear.out" (run rule will compile
if needed as well.

Run through make file using default rule (run). Specify number of bees through
setting BEES to the amount of bees wanted.

Example:
> BEES=30 make

The bees will gather honey untill the pot is full as specified in the assignment and then
the bee that fills the pot will wake the bear. This is done through the use of 4 semaphores.
One semaphore is used for the critical section (updating the counters etc) and then 2 
semaphores is used to wait for the pot to be either full or empty.

When a bee fills the pot it will still have the crtical section semaphore. This semaphore will
be passed on to the bear when waking the bear. Therefore there cannot be a bear and a bee
performing actions on the counters at the same time. Once the bear is done it will pass the
critical section semaphore back to the bee waking it.

If the bear has not yet arrived to the spot where it waits for the pot to be full when a bee
has filled the pot this is not an issue as the semaphores will then be pot_full = 1 and 
pot_empty = 0. This will cause the bee to wait a bit more time than usual but there are no race
condition that could screw up the runtime.

The solution is fair in both regards to the honeybees and to the bear. This is because they will
queue to enter the honey in the pot with the critical section semaphore and we can assume that the 
semaphore to be fair. There is no busy waiting. Hence all the bees will be able to put honey in the 
pot. The bear will get to eat the honey as soon as the pot is full. This makes the solution fair to
both the producers (bees) and the consumers (bear).

Example output from the simulation can be found in the file example.txt.
