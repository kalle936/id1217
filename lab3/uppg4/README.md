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

To achieve this both the males and females use a technique called pass the baton to control
who gets to enter. Otherwise there could be starvation due to one gender always getting control
of the required semaphore. With passing the baton technique we can control that if males has exited
females can now enter and we pass the critical section semaphore when we alert the head of the
female queue. This works the other way around as well. 

In fact male and female logic are extremely similair, the only difference is the semaphore and
counters it uses at different times. 

The outline for the male and female run inside the loop is:

    * Get hold of critical section semaphore.
    * Try to enter:
        If there are another person of the opposite gender inside or waiting
            Enter queue for the appropriate gender. (male -> waiting_male_sem, 
            female waiting_female_sem). Once we get hold of the semaphore we 
            check if we are the head of the queue, if we were we let the rest 
            of the queue enter as well.
        Else
            Enter the bathroom.
    * Release the critical section semaphore.
    * Do your business inside the bathroom.
    * Get hold of critical section semaphore.
    * Exit the bathroom
    * If we were the last to exit the bathroom we let the head of the queue
      (if there are any) of the opposite gender know that it is their turn 
      to use the bathroom.
    * Release the critical section semaphore.


Example run could be

* Male enter. /* Nobody inside, male can enter. */
* Male enter. /* Males inside and no females waiting, hence male can enter. */
* Female enter queue. /* Female cant enter because males are inside. */
* Male enter queue. /* Male cant enter due to there is a female waiting. */
* Female enter queue. /* Female cant enter because males are inside. */
* Male exits. /* One male still in bathroom. */
* Male exits. /* Last male exits and lets the head of the queue of females waiting know. */
* Female enter. /* Female enter and lets the other female in the queue know that they can 
                enter aswell.*/
* Female enter. /* Other female enter aswell after being told they can enter. */
* Female exit. /* One female still in the bathroom. */
* Female exit. /* Last female exit the bathroom and lets the head of the male queue kow they
              can enter aswell. */
* Male enter. /* Male enter. */
* Male exit. /* Male exit. */


As we can see the second female can still enter the bathroom even though there is
a male in the queue. This increases the efficiency of the bathroom significantly.

The solution is still fair because the waiting time is at most almost 2 turns of
using the bathroom. 

Worst case scenario described:

* Male enter.
* Female enter queue.
* Male enter queue. <- Us
* Male exit. <-- Males exit their turn.
* Female enter. <-- Females begin their turn.
* Female exit. <-- Females exit their turn.
* Male enter. <-- It is now our turn.
* ...

Of course in this scenario any number of males and females in the two turns can use the
bathroom. However as they can all use the bathroom in parallel this does not matter in
respect of fairness.

There is no way of starvation in this implementation. 


In the zip-file there are two example runtimes. They are called example1.txt and example2.txt.
The examples were created using 10 males and 10 females for a minimum 20 times in the bathroom.
