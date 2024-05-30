#ifndef Distribute
#define Distribute
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<omp.h>
#define Graph int**
#define ll long long
struct distribute{
    int SIZE, RANK;
    int vertices;
    int* Parents;
    Graph localGraph;
};
void InitDistribute(struct distribute *d,int s,int r,int v)
{
        d->vertices = v;
        d->SIZE= s;
        d->RANK= r;
}
    // functions and constructors
    
void BroadCastOwners(struct distribute *d)
{
    int* owners=(int*) malloc (2*d->SIZE*sizeof(int));
    // definin ceil division
    int NodesPerProc = d->vertices/ d->SIZE;
    int start = 0;
    if(d->RANK == 0)
    {
        for(int i=0;i<d->SIZE;i++)
        {
            owners[2*i] = start;
            owners[2*i + 1] = NodesPerProc;
            start+=NodesPerProc;
            if(i < (d->vertices%d->SIZE))
            {
                owners[2*i+1]++;
                start++;
            }
        }
    }
    //Distribution Done
    MPI_Bcast(owners, 2*d->SIZE,MPI_INT,0,MPI_COMM_WORLD); 
    d->Parents = owners;

        
}
void ScatterValues(struct distribute *d,Graph graph)
{
    // newgraph is obtained by joining the graphs
    int* newGraph = (int*)malloc(d->vertices * d->vertices * sizeof(int));
    int* SumOfSizes =(int*)malloc(d->SIZE*sizeof(int));
    int* PrefixSum= (int*)malloc(d->SIZE*sizeof(int));
    if(d->RANK == 0)
    {

        int id=0;
        PrefixSum[0]=0;
        for(int i=0;i<d->SIZE;i++)
        {  
            SumOfSizes[i]=0;
            for(int j=d->Parents[2*i];j<d->Parents[2*i+1] +d->Parents[2*i]; j++)
            {

                for (int k=0;k<d->vertices;k++)
                {
                    newGraph[id++] =  graph[j][k];
                }
                SumOfSizes[i]+=d->vertices;
            }
            if(i < d->SIZE-1)
                PrefixSum[i+1]= PrefixSum[i]+SumOfSizes[i];
        }

       
    }

    // Received part from main proc;
    int sizeReceived=d->Parents[2*d->RANK + 1] * d->vertices;
    int *ReceivedVector= (int*)malloc(sizeof(int)*sizeReceived);
    MPI_Scatterv(newGraph,SumOfSizes,PrefixSum,
        MPI_INT,ReceivedVector,sizeReceived,MPI_INT,0,MPI_COMM_WORLD);
    d->localGraph = (int**)malloc(sizeof(int*)*d->Parents[2*d->RANK + 1]);
    int idx = 0;
    for(int i=0;i<d->Parents[2*d->RANK + 1];i++)
    {
        d->localGraph[i] = (int*)malloc(sizeof(int)*d->vertices);
        for(int j=0;j<d->vertices;j++)
            d->localGraph[i][j] = ReceivedVector[idx++];
    }
}
void DistributeGraph(struct distribute *d,Graph graph)
{
    BroadCastOwners(d);
    ScatterValues(d,graph);
}
#endif