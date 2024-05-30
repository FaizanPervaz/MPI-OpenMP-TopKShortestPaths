The `.cpp` file contains the serial implementation of the algorithm, while the `.c` file contains the OpenMPI + OpenMP implementation.

To compile the `.c` file, use the following command:

```bash
mpicc -fopenmp -o myobj <filename>.c
```

Once the file is compiled we can define the number of processes and execute the object file with following command

```bash
mpiexec -n <number of process> ./myobj
```
