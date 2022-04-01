# Simulated Annealing
Simulated Annealing is the process by which global optimum's can be approximated in a given function. It resembles the real-world process of annealing in metallurgy, where a heated metal is brought up to a high tempurature where larger changes are made to it's physical properties, and then back to a cooled temperature where less drastic changes are made to it's physical properties, all in a controlled manner.

In *Simulated* Annealing, the changes we make at a given temperature can be considered a measure of the *randomness* by which changes are made, thus seeking to *minimize* these changes. In the context of our 8-Puzzle proble, this change is in the amount of 'worse' moves can be made. In the context of the Traveling Salesman problem, this is the amount by which changes are made to the path.

# Traveling Salesman
The Traveling Salesman problem is one of the most important in the study of *optimization*. The problem statement is as follows:

> Given an initial list of towns and, as well as their locations represented as cartesian coordinates, find the shortest path the salesman has to walk to visit every town exactly once and return to his origin (the randomly selected town they started in).

This problem is a perfect candidate for simulated annealing! We start by taking a random initial path, then comparing it to a random 'change' in this path using our change in tempurate at each iteration to reach a more and more optimized path

In this program, there will not be a pre-defined goal state like in the *8-Puzzle* problem. We will simple iterate using our tempurature $T$ as before, and exit if either of the following conditions are met:
- The temperature $T = T_{min}$ 
- The heuristic cost of the current path $H_{current}$ is lower than that of any of the changes to the path we attempted to compare it to. 

We will use a new heuristic in this program, the **Euclidean Distance**, to evalutate the cost of each path from start to finish. For our cooling function, we will decrease the value of our tempurature **exponentially**.

With each iteration of our tempurature, we will select a segment (start and end town) of our path at random. Then, we will 'flip a coin' and decide wether or not to **reverse** the segment, or **transport** the segment.
- In **reversing** a segment, we will *reverse* the segment in order of towns visited, and then place it back into our path (as a new path) and compare it with the original configuration.
- In **transporting** a segment, we will *transport* this segment into a different, randomly chosen position in our path (as a new path) and compare it with the orignal configuration.

## Implementation
This program will be implemented using **C++** to take advantage of the Standard Template Library's *vector* class, among other features.

#### 1. Create a Random Initial State
Here we'll want to have an array (vector in C++) of towns representing an initial path the salesman takes. Each town will be represented by cartesian coordinates.

The data structures defined for these objects can be observed below:
```cpp
/* A town, represented by cartesian coordinates */
struct Town {
	int x, y;
}

/* The path of our salesman */
struct Path {
	vector<Town> Towns; // The list of (x,y) coordinates of each town
	double H_Cost;      // The total heuristic cost of all towns in the path
}
```

#### 2. Define the Base Cases
As stated above, our base cases are as follows:
- If the temperature $T = T_{min}$ , display the current board at that temperature as the solution.
- If the heuristic cost of the current path $H_{current}$ is lower than that of any of the changes to the path we attempted to compare it to, display the current path as the solution.

> Note: obviously, even the most powerful computers don't have the capability to test every conceivable random path at every iteration of $T$. So, we must set the *total number of changes able to made to a given path* to some arbitrary value. Here, that is 100 times the number of towns in the original path.

```cpp
// Iterate until tempurature reaches its minimum value using exponential cooling
for (double T = T_max; T > T_min; ExpCooling(&T, T_step)) {
	bool pathFound = false; // If a more efficient path is found
	int compareSteps = 100 * &path->length; // The arbitrary number of comparisons.
	while(compareSteps >= 0 && pathFound == false) {
		...
		double H_delta = H_new - H_current;
		if (H_delta < 0) {
			p_current = p_new;
			pathFound = true;
		} else if (exp((-H_delta)/ T) > rand(0,1)) {
			p_current = p_new;
			pathFound = true;
		} else if (compareSteps == 0) {
			PrintVector(p_curr.towns);
			exit(EXIT_SUCCESS);
		} 
		else {
			compareSteps--;
			continue;
		}
		...
	}
}
```


#### 3. Choose a Cooling Function
For this example I will be using an arbitrary starting tempearture of $T_{max} = 2.0$ and an arbitrary minimum temperature of $T_{min} = 0.09$. I will attempt to use the **Exponential Cooling Function**, which will decrease the tempurature using the following equation:$$\Delta T = T_{max} \times e^{-T_{step}} $$ Where our *decay constant* $T_{step}$ will be $0.1$. This value is decided on as our decay constant so that, for every iteration, $T$ will equal *90%* the value of the previous temperature $T$.

```cpp
/* Exponential Cooling Function */
void ExpCooling(double* T, double T_step) {
	*T = *T * exp(-T_step);
}
```

#### 4. Choosing a Cost Function $H()$
Here we will use **Euclidean Distance** which, unlike manhattan distance, will look at the *diagonal* distance from each cell, simulated a more accurate path than just using right-angle distances.$$H(x,y) = \sqrt{(\vert x_2 - x_1\vert)^2+(\vert y_2 - y_{1}\vert)^2}$$
```cpp
/* Hueristic Function - Euclidean Distance*/
double EuclideanDistance(Path* p) {
	double cost = 0;	// Total heuristic cost of this path

	PrintVector(p->towns);

	// Iterate through each town in the path
	for (auto it = p->towns.begin(); it != p->towns.end() - 1; ++it) {
		Town* t1 = &(*it);
		Town* t2 = &(*std::next(it, 1));

		double x_delta = abs(t2->x - t1->x);
		double y_delta = abs(t2->y - t1->y);
		double ed = sqrt((x_delta * x_delta) + (y_delta * y_delta));

		cost += ed;
	}

	return cost;
}
```

#### 5. Applying the Probability Function $e^{\frac{-\Delta H}{T}}$
This process is practically identical to how we applied it for the **8-Puzzle** problem. If, when comparing two paths, the heuristic cost of the current path is *greater* than that of the new path, we must use our probability function:$$e^{\frac{-\Delta H}{T}} > rand(0,1)$$
This function is, again, important because it uses the changing value of our temperature $T$ to decide if a *worse* path can still be used

```cpp
...
else if (exp((-H_delta)/ T) > rand(0,1)) {
			p_current = p_new;
			pathFound = true;
}
...
```
