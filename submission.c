#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include"distribute.h"
#include"KShortestPaths.h"
#include<omp.h>
#include <time.h>
// definign our constants
const int VERTICES = 694;
// creating a class for parallizing the
Graph GetGraph()
{
    Graph temp = (int**)malloc(sizeof(int*)*VERTICES);
    for(int i=0;i<VERTICES;i++)
    {
        temp[i]= (int*)malloc(sizeof(int)*VERTICES);
        for(int j=0;j<VERTICES;j++)
            temp[i][j] = 1e9;
        temp[i][i] = 0;
    }

    FILE *file;
    int number;
    file = fopen("input.txt", "r");
    while (fscanf(file, "%d", &number) != EOF) {
        int u = number,v=0,w=0;
        fscanf(file,"%d",&v);
        fscanf(file,"%d",&w);
        temp[u-1][v-1] = w;
    }
    // for(int i=0;i<VERTICES;i++)
    // {
    //     printf("%d : ",i+1);
    //     for(int j=0;j<VERTICES;j++)
    //     {
    //         printf("%d ",temp[i][j]);
    //     }
    //     printf("\n");
    // }
    fclose(file);
    return temp;
}
int main(int argc,char *argv[])
{
    srandom(time(NULL));
    int size,rank;
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Graph graph;
    // setting the number of threads
    omp_set_num_threads(10);
    int arr1[10] = {0};
    int arr2[10] = {3};
    int K;
    if(rank == 0)
    {    
        graph = GetGraph();
        printf("Enter How many shortest Paths you need ( K ) : ");
        scanf("%d",&K);
        for(int i=1;i<10;i++)
        {
            arr1[i] = rand()%VERTICES;
            arr2[i] = rand()%VERTICES;
        }
    }
    MPI_Bcast(arr1,10,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(arr2,10,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&K,1,MPI_INT,0,MPI_COMM_WORLD);
    struct distribute para;
    InitDistribute(&para,size,rank,VERTICES);
    DistributeGraph(&para,graph);
    for(int i=0;i<10;i++)
    {
        struct P_KSP paths;
        InitP_KSP(&paths,para.RANK,para.SIZE,para.Parents,para.localGraph,arr1[i],arr2[i],VERTICES);
        FindPaths(&paths,K);
    }

    MPI_Finalize();
    return 0;
}