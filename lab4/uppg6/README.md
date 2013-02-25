6. The Bear and the Honeybees Problem (20 points)

This program simulates the behaviour of a specified amount of honeybees and
a bear. The bear eats honey from a pot, that the bees fills up, once it is full.
When the bear has eaten honey a fixed number of times the simulation exits.

In this implementation the interaction with the animals and the honey pot
is done through a monitor. This monitor is described in monitor.h and
monitor.cpp. It has 2 different methods that is used for interacting with
the hoeny pot. These methods are fill_pot and eat_honey. Fill_pot will try
to put a portion of honey in the honey pot, if the honey pot is full it will
signal to the bear to eat the honey and wait for the pot to be empty. Eat_honey
is the method used by the bear to eat the honey once the honey pot is full, if it
is not full it will wait for the bees to signal that the honey pot is full. Once
eaten the bear will signal to any bees waiting that the honey pot is now empty.

This solution is as fair as it can be done with locks. The one who gets hold of
the lock first gets to place honey in the pot first. If a bee has to wait for
the pot to be empty before it can place its honey in the pot it will need to
reaccuire the lock again after the bear has signaled that the pot is empty. This
causes everything to have the same bottleneck - the lock. However, the solution is
fair because it is guaranteed that the number of bees present in the simulation is
lesser than the amount of portions of honey that the pot can take. This results in
that every bee that wants to put honey in the pot will be able to do so - no starvation!

Run the program through make. (Default rule.) Specify number of bees by specifying BEES.

Examlpe:
> BEES=15 make

Compile it through make rule feedthebear. This will compile the object file of the monitor
and the program feedthebear which is the name of this simulation.

Example:
> make feedthebear

Example output from a run can be found in file example1.
