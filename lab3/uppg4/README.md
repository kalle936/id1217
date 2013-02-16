4. The Unisex Bathroom Problem (40 points).

Unisex toilet simulation with males and females represented as
threads.

Compile by using make file rule unisex.out. (Compiling is
also done on demand when running the program.)

Run the program through default make rule. (run)
To specify number of males set NRM and to specify number of 
females set NRW.

Example usage: 
> NRM=10 NRW=10 make


The males and females are trying to enter a single bathroom to do their
business. Only one gender can access the bathroom at the time. However, there
are unlimited number of toilets in the bathroom meaning that any number of the
same gender can use the bathroom at the same time.

Example run could be

Male enter.
Male enter.
Female enter queue.
Male enter queue.
Female enter queue.
Male exits.
Male exits.
Female enter.
Female enter.
Female exit.
Female exit.
Male enter.
Male exit.

As we can see the second female can still enter the bathroom even though there is
a male in the queue. This increases the efficiency of the bathroom significantly.

The solution is still fair because the waiting time is at most almost 2 turns of
using the bathroom. 

Worst case scenario described:

Male enter.
Female enter queue.
Male enter queue. <- Us
Male exit. <-- Males exit their turn.
Female enter. <-- Females begin their turn.
Female exit. <-- Females exit their turn.
Male enter. <-- It is now our turn.
...

Of course in this scenario any number of males and females in the two turns can use the
bathroom. However as they can all use the bathroom in parallel this does not matter in
respect of fairness.

There is no way of starvation in this implementation. 


In the zip-file there are two example runtimes. They are called example1.txt and example2.txt.
The examples were created using 10 males and 10 females for a minimum 20 times in the bathroom.
The examples were created using 10 males and 10 females for a minimum 20 times in the bathroom.

