// C++ implementation of above approach

#include<algorithm>
#include <iostream>
#include<fstream>
#include<vector>
#include<queue>
using namespace std;


void ReadFile(vector<vector<pair<int,int>>> & graph)
{
    ifstream file;
    file.open("input.txt");
    int u, v, w;
    while (file >> u >> v >> w) {
        // Assuming `graph` is defined somewhere in your code
        graph[u].push_back({v, w});
    }
}
// Function to find K shortest path lengths
void findKShortest(int n, int k, int start,int end)
{

	// Initialize graph
	vector<vector<pair<int, int> > > g(n + 1);
    ReadFile(g);

	// Vector to store distances
	vector<vector<int> > dis(n + 1, vector<int>(k, 1e9));

	// Initialization of priority queue
	priority_queue<pair<int, int>, vector<pair<int, int> >,
				greater<pair<int, int> > >
		pq;
	pq.push({ 0, start });
	dis[start][0] = 0;

	// while pq has elements
	while (!pq.empty()) {
		// Storing the node value
		int u = pq.top().second;

		// Storing the distance value
		int d = (pq.top().first);
		pq.pop();
		if (dis[u][k - 1] < d)
			continue;
		vector<pair<int, int> > v = g[u];

		// Traversing the adjacency list
		for (int i = 0; i < v.size(); i++) {
			int dest = v[i].first;
			int cost = v[i].second;

			// Checking for the cost
			if (d + cost < dis[dest][k - 1]) {
				dis[dest][k - 1] = d + cost;

				// Sorting the distances
				sort(dis[dest].begin(), dis[dest].end());

				// Pushing elements to priority queue
				pq.push({ (d + cost), dest });
			}
		}
	}

	// Printing K shortest paths
	for (int i = 0; i < k; i++) {
		cout << dis[end][i] << " ";
	}
}

// Driver Code
int main()
{

	// Given Input
	int N = 694, K = 3;
    cout <<"Enter the number of shortest paths you need \n";
    cin >> K;
    for(int i=0;i<10;i++)
    {
        int u =rand()%N +1 , v = rand()%N+1;
	    cout << u <<" to " << v <<" = ";
        findKShortest(N, K ,u,v);
        cout <<endl;
    }
	
    cout << 1 <<" to " << 4 <<" = ";
    findKShortest(N, K ,1,4);


	return 0;
}
