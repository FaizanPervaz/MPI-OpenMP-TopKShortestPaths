


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"distribute.h"
#include<stdbool.h>
struct P_KSP{
    Graph localGraph;
    int start;
    int ValuesContained;
    int RANK;
    int SIZE;
    int SNode,Dnode;
    int *Parents;
    int *dpParents;
    int vertices;

}; 
struct PriorityQueue{
    ll * indices;
    ll * values;
    int index;
    int size;

};

// Function to initialize the priority queue
void InitPQ(struct PriorityQueue *q, int size) {
    q->size = size;
    q->index = 0;
    q->indices = (ll*)malloc(sizeof(ll) * size);
    q->values = (ll*)malloc(sizeof(ll) * size);
    
    
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        q->indices[i] = q->values[i] = -1;
    }
}

// Function to push an element into the priority queue
void pushintopq(struct PriorityQueue *q, ll node, ll value) {
    int idx = q->index++;
    q->indices[idx] = node;
    q->values[idx] = value;
}

// Function to check if the priority queue has value
bool HasValue(struct PriorityQueue *q) {
    return q->index > 0;
}

// Function to extract the minimum element from the priority queue
ll* ExtractMin(struct PriorityQueue *q) {
    int min_value = q->values[0];
    int min_index = q->indices[0];
    int id=0;
    for (int i = 0; i < q->index; i++) {
        if (min_value > q->values[i]) {
            min_value = q->values[i];
            min_index = q->indices[i];
            id =i;
        }
    }
    q->indices[id] = q->indices[q->index - 1];
    q->values[id] = q->values[q->index - 1];
    q->index--;
    
    ll *ret = (ll*)malloc(sizeof(ll) * 2);
    ret[0] = min_index;
    ret[1] = min_value;

    return ret;
}
void insertAndRemoveLargest(long long *arr, int n, long long newElement) {
    // printf("before insertion : ");
    // for(int i=0;i<n;i++)
    //     printf("%lld ",arr[i]);
    //     printf("\n");
    arr[n-1]= newElement;
    int j = n - 2;
    while (j >= 0 && arr[j] > newElement) {
        arr[j + 1] = arr[j];
        j--;
    }
    arr[j + 1] = newElement;
    // printf("after insertion %lld: ",newElement);
    // for(int i=0;i<n;i++)
    //     printf("%lld ",arr[i]);
    //     printf("\n");
    

}
void InitP_KSP(struct P_KSP *P,int rank,int size,int *Parents,int **LocalGraph
    ,int snode,int enode,int VERTICES)
{
    P->vertices = VERTICES;
    P->Parents = Parents;
    P->SNode = snode;
    P->Dnode = enode;
    P->RANK = rank;
    P->SIZE = size;
    P->ValuesContained = Parents[2*rank + 1];
    P->start = Parents[2*rank];
    P->localGraph = LocalGraph;
    P->dpParents=(int*)malloc(sizeof(int)*VERTICES);
    #pragma omp parallel for schedule(dynamic)
    for(int i=0;i<P->SIZE;i++)
    {

        for(int j=Parents[i*2];j<Parents[i*2]+Parents[i*2+ 1];j++)
            {
                P->dpParents[j] = i;
            }
    }
}
void FindPaths(struct P_KSP *p,int K)
{
    struct PriorityQueue pq;
    ll **dist = (ll**)malloc(sizeof(ll*)*p->ValuesContained);
    #pragma omp parallel for schedule(dynamic)
    for(int i=0;i<p->ValuesContained;i++)
    {
        dist[i] = (ll*)malloc(sizeof(ll)*K);
        for (int j=0;j<K;j++)
            dist[i][j] = 1e18;
    }
    InitPQ(&pq,p->vertices * p->vertices);
    if(p->dpParents[p->SNode] == p->RANK)
    {
        insertAndRemoveLargest(dist[p->SNode - p->start],K,0);
        pushintopq(&pq,p->start,0);
    }

    // for(int tttt=0;tttt<2;tttt++)
    while(true)
    {
        int * counters = (int*)malloc(sizeof(int)*p->SIZE);
        ll* costs = (ll*)malloc(sizeof(ll)*p->SIZE*p->vertices);
        ll* indices = (ll*)malloc(sizeof(ll)*p->SIZE*p->vertices);
        int idx=0;
        for(int i=0;i<p->SIZE;i++)
        {
            counters[i]=0;
            for(int j=0;j<p->vertices;j++)
                costs[i*p->vertices+ j ] = indices[i*p->vertices + j] = -1;

        }

        // printf("%d has %d in queue \n",p->RANK,pq.index);
        if(HasValue(&pq)==true)
        {
            ll* ret = ExtractMin(&pq);
            // printf("Rank %d has extracted (%d,%d)\n",p->RANK, (int)ret[0],(int)ret[1]);
            ll u = ret[0];
            ll cost = ret[1];
            // printf("cost = %d  and the orig values is: %d\n",cost,dist[(int)u - p->start][K-1]);
            if(cost <= dist[(int)u - p->start][K-1])
            {
                
                int* iter = p->localGraph[(int)u - p->start];
                #pragma omp parallel for schedule(dynamic)
                for(int i=0;i<p->vertices;i++)
                {   
                    if(iter[i]!=(int)1e9 && u!=i)
                    {
                        int par =p->dpParents[i];
                       #pragma omp critical
                        {
                            costs[par*p->vertices + counters[par]] = cost + (ll)iter[i];
                            indices[par*p->vertices + counters[par]++] = i;
                        }
                    }
                }
            }
        }

        // for(int i=0;i<p->SIZE;i++)
        // {
        //     printf("rank s%d : %d :",p->RANK,i);
        //     for(int j =0;j<p->vertices;j++)
        //         printf("(%d,%d )",costs[i*p->vertices + j],indices[i*p->vertices + j]);
        //     printf("\n");
        // }

        // all buffers are full now we will do alltoall pew pew;
        int * ncounters = (int*)malloc(sizeof(int)*p->SIZE);
        ll* ncosts = (ll*)malloc(sizeof(ll)*p->SIZE*p->vertices);
        ll* nindices = (ll*)malloc(sizeof(ll)*p->SIZE*p->vertices);

        MPI_Alltoall(costs,p->vertices,MPI_LONG_LONG_INT , ncosts,p->vertices
        ,MPI_LONG_LONG_INT , MPI_COMM_WORLD);

        MPI_Alltoall(indices,p->vertices,MPI_LONG_LONG_INT , nindices,p->vertices
        ,MPI_LONG_LONG_INT , MPI_COMM_WORLD);

        MPI_Alltoall(counters,1,MPI_INT , ncounters,1
        ,MPI_INT , MPI_COMM_WORLD);

        // for(int i=0;i<p->SIZE;i++)
        // {
        //     printf("rank R %d : %d :",p->RANK,i);
        //     for(int j =0;j<p->vertices;j++)
        //         printf("(%d,%d )",ncosts[i*p->vertices + j],nindices[i*p->vertices + j]);
        //     printf("\n");
        // }


        // all received at the correct parent
        //updates values and adds into priority queue;
        #pragma parallel for schedule(dynamic)
        for(int i=0;i<p->SIZE;i++)
        {
            for(int j=0;j<ncounters[i];j++)
            {
                int org = nindices[i*p->vertices + j];
                int newInd = org - p->start;
                if(ncosts[i*p->vertices+j] < dist[newInd][K-1])
                {
                    #pragma omp critical
                    {
                        // printf("rank %d setting values of %d(%d) to %d\n",p->RANK,org,newInd,ncosts[i*p->vertices+j]);
                        insertAndRemoveLargest(dist[newInd],K,ncosts[i*p->vertices+j]);
                        pushintopq(&pq,org,ncosts[i*p->vertices+j]);
                    }
                }
            }
        }
        int sz = pq.index;
        // printf("size = %d\n",sz);
        int nsz =0;
        MPI_Allreduce(&sz,&nsz,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
        if(nsz==0)
            break;
    }
    if(p->dpParents[p->Dnode] == p->RANK)
    {
        printf("top %d paths from %d to %d are : ",K,p->SNode,p->Dnode);
        for(int i=0;i<K;i++)
            printf("%d , ",dist[p->Dnode - p->start][i]);
        printf("\n");
    }
}