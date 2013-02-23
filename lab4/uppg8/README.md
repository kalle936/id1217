8. The Unisex Bathroom Problem (40 points)

Simulation of the unisex bathroom using monitors to handle entering and exiting of the bathroom.
The rules of this simulation are that there are unlimited toilets inside the bathroom but only
one gender should be able to enter the bathroom at a time. The solution have to be fair and no
deadlocks should be possible.

All the logic of entering and exiting the bathroom is inside the monitor class. The syncronization
is performed using one lock and two different condition variables. One condition variable is used to
signal to the men to enter and one is used to signal to the women to enter. These condition variables
are broadcasted by the last person to exit the bathroom before a change of gender in the unisex
bathroom. (Last male calls the woman condition variable and vice versa.)

The solution is fair because there absolute maximum time any person need to wait is 2 rounds inside the
bathroom. This happens when you come to the bathroom and there is still persons of your gender inside the
bathroom and there are one or more persons of the opposite gender waiting to enter. To ensure fairness you
will have to wait for the opposite gender to enter the bathroom before you can enter. Therefore you will
have to first wait for the rest of the persons inside the bathroom and then of the persons of the opposite
gender waiting to enter the bathroom as well, then it is your turn. No deadlock can occur because the
Coffman condition of circular wait can never arise.
