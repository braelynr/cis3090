Author: Braelyn Rotman
Student ID: 1006740

To compile: make
To run serial program: ./maze -n size [-s seed]
To run parallel program: ./mazep -n size [-s seed]

On very large maze sizes the parallel program (mazep) occasionally results in a segmentation fault
 although I believe this is due to some school server issue as it only occurs during mid day.
 If this occurs please just execute again.

On smaller maze sizes thread 0 often claims ost or all the spaces. This is normal not a bug
