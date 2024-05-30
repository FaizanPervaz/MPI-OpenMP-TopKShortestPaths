# MPI-OpenMP-TopKShortestPaths

This repository contains a parallel implementation of the Top K Shortest Path Problem combining MPI for distributed computing across multiple nodes and OpenMP for shared memory parallelization within each node.

## Project Overview
The project aims to solve the Kth Shortest Path Problem, where instead of just finding the shortest path in a graph, we seek the Kth shortest path between two nodes. This problem is more complex and has significant applications in network design, traffic routing, and more.

## Graph Datasets
The implementation is tested with various real-world graphs:
- Doctor Who https://github.com/manuel-dileo/doctor-who-dataset/tree/main
- Enron Email https://snap.stanford.edu/data/email-Enron.html
- EU Email https://snap.stanford.edu/data/email-EuAll.html

## Features
- **MPI**: Utilizes the Message Passing Interface for distributed computing across multiple computing nodes.
- **OpenMP**: Uses OpenMP for parallelizing computations within each node to leverage multi-core processors efficiently.
- **Dynamic Workload Distribution**: Dynamically assigns tasks to processes to balance the workload and optimize resource usage.

## Building and Running
### Prerequisites
- MPI (e.g., MPICH, OpenMPI)
- OpenMP supported compiler (e.g., GCC)

### Compilation
The `.cpp` file contains the serial implementation of the algorithm, while the `.c` file contains the OpenMPI + OpenMP implementation.
To compile the `.c` file, use the following command:
```bash
mpicc -fopenmp -o myobj <filename>.c
```
Once the file is compiled we can define the number of processes and execute the object file with following command
```bash
mpiexec -n <number of process> ./myobj
```
