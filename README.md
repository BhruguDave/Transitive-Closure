# Transitive-Closure
This repository contains three parallelized algorithms to find the Transitive Closure of a given input graph on shared memory systems using the OpenMP API in C++. The three algorithms are:
* V1 : Adjacency list based implementation (Note: This implementation takes execution times in the order of hours)
* V2 : Adjacency matrix based implementation
* V3 : Cache-optimized adjacency matrix based implementation
## File and Directory structure
* datasets - contains the experimental datasets in *.csv* format
* main.cpp - the main source code for the algorithms and running the experiments to find the transitive closure
## Steps to run the code
* Clone the repository to the local system.
    * #### `git clone https://github.com/BhruguDave/Transitive-Closure.git`
* From the command line tool, change the directory to the `Transitive-Closure`  </br>

* Use the following command to compile the code:
    * #### `g++ main.cpp -O3 -fopenmp -o main.out`
    * #### Note: If you want to compile using `gcc`, use the following command: 
        * #### `gcc main.cpp -O3 -lstdc++ -fopenmp -o main.out`
* Use the following command to run the code:
    * #### `./main.out`
## Experiment customization
* To select/deselect any algorithm from V1, V2 and V3, uncomment/comment the lines 30, 31 and 32 respectively in *main.cpp*.
* To run the serial code as well, make sure the variable `runSerial` is set to `true` in line 34 of *main.cpp*, else change it to false.
* Change the number of threads to be used for the experiment by adding/removing the same from the list `numberOfProcesses` on line 36 of *main.cpp*.
* To change the datasets for the experiments, uncomment/comment the lines 41 to 52 of *main.cpp* to select/deselect the datasets for execution in the experiment.
## Contributors
* Sarthak Patel - 201801435@daiict.ac.in
* Bhrugu Dave - 201801401@daiict.ac.in
* Smit Kumbhani - 201801404@daiict.ac.in
* Mihir Desai - 201801033@daiict.ac.in
