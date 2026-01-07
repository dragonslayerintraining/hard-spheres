# Hard spheres

randomness-recycler.cpp contains an implementation of a perfect sampling algorithm for the 2D hard sphere model, based on a variant of the randomness recycler (https://arxiv.org/abs/math/0009242).

The algorithm takes in a graph and activity and outputs a random independent set from the hard core model on this graph at this activity.

The graph is given as a set of oracles that return: the total measure of the graph, a random vertex of the graph, the measure of the neighborhood of a given vertex, a random neighbor of a given vertex, and whether two given vertices are adjacent. This allows the graph to be infinite, which is the case for the hard disk model. For the hard disk model, the vertex set is the torus [0,1]x[0,1], and two vertices are considered adjacent if their distance is less than twice the radius of the disks.

For disks of radius 0.1 on the unit torus, it can handle activity up to around 0.35 easily, and then drastically slows down beyond that.

The implementation has been tested by using it to estimate the expected size of the random independent set on various 4-vertex graphs and comparing it to the exact value.


