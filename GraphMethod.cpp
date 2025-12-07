#include <iostream>
#include <vector>
#include "GraphMethod.h"
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <list>
#include <utility> // for pair, make_pair
#include <algorithm>
#include <fstream>
#include <climits>
#include <iomanip>
#include <cmath>

using namespace std;

// Used to Initialize Shortest Path Algorithm
const int INF = 100000000;

// edge struct
struct Edge
{
    int u, v, weight;
    // Weighted ascending order
    bool operator<(const Edge &other) const
    {
        if (weight != other.weight)
            return weight < other.weight;
        if (u != other.u)
            return u < other.u;
        return v < other.v;
    }
};

// Union-Find Data Structure for Cycle Discrimination in Kruskal Algorithm
struct DisjointSet
{
    vector<int> parent; // parent node

    DisjointSet(int n)
    {
        parent.resize(n);
        for (int i = 0; i < n; i++)
            parent[i] = i;
    }

    int find(int u) // Return top root node using Path Compression
    {
        if (u == parent[u])
            return u;
        return parent[u] = find(parent[u]);
    }

    void merge(int u, int v) // Combine two sets into one
    {
        u = find(u);
        v = find(v);
        if (u != v)
            parent[v] = u;
    }
};
// Use a queue to visit from the nearest vertex to the starting vertex in turn
bool BFS(Graph *graph, char option, int vertex)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();
    // No graph, or the starting vertex is out of valid range
    if (vertex < 0 || vertex >= size)
        return false;

    ofstream fout("log.txt", ios::app);
    vector<bool> visited(size, false);
    queue<int> q;

    q.push(vertex);
    visited[vertex] = true;

    // output format
    fout << "========BFS========" << endl;
    if (option == 'O')
        fout << "Directed Graph BFS" << endl;
    else
        fout << "Undirected Graph BFS" << endl;

    fout << "Start: " << vertex << endl;
    fout << vertex;

    // Until queue is empty
    while (!q.empty())
    {
        int curr = q.front();
        q.pop();

        map<int, int> adj;
        // Import directional/non-directional neighbor vertices according to options
        if (option == 'O')
            graph->getAdjacentEdgesDirect(curr, &adj);
        else
            graph->getAdjacentEdges(curr, &adj);

        // Adjacent vertex traversal
        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int next = it->first;
            // if not visited, add it to the queue and process the visit
            if (visited[next] == false)
            {
                visited[next] = true;
                q.push(next);
                fout << " -> " << next;
            }
        }
    }
    fout << "\n=====================\n\n";
    fout.close();
    return true;
}
// Use Stack to explore depth first
bool DFS(Graph *graph, char option, int vertex)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();
    if (vertex < 0 || vertex >= size)
        return false;

    ofstream fout("log.txt", ios::app);
    vector<bool> visited(size, false);
    stack<int> s; // Stack for navigation

    s.push(vertex);

    fout << "========DFS========" << endl;
    if (option == 'O')
        fout << "Directed Graph DFS" << endl;
    else
        fout << "Undirected Graph DFS" << endl;
    fout << "Start: " << vertex << endl;

    bool first = true;
    while (!s.empty())
    {
        int curr = s.top();
        s.pop();
        // Pass if you have already visited a peak
        if (visited[curr] == true)
            continue;

        visited[curr] = true;

        if (first)
        {
            fout << curr;
            first = false;
        }
        else
        {
            fout << " -> " << curr;
        }

        map<int, int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(curr, &adj);
        else
            graph->getAdjacentEdges(curr, &adj);

        // put it in reverse order so that the smaller number comes out
        map<int, int>::reverse_iterator rit;
        for (rit = adj.rbegin(); rit != adj.rend(); rit++)
        {
            int next = rit->first;
            if (visited[next] == false)
            {
                s.push(next);
            }
        }
    }
    fout << "\n=====================\n\n";
    fout.close();
    return true;
}
// an edge-centered grid algorithm, KRUSKAL
bool Kruskal(Graph *graph)
{
    if (graph == NULL)
        return false;

    int size = graph->getSize();
    vector<Edge> edges;

    for (int i = 0; i < size; i++)
    {
        map<int, int> adj;
        graph->getAdjacentEdges(i, &adj);
        // Collect all edges of the graph
        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int v = it->first;
            int w = it->second;
            // Avoid redundant edges
            if (i < v)
            {
                Edge newEdge;
                newEdge.u = i;
                newEdge.v = v;
                newEdge.weight = w;
                edges.push_back(newEdge);
            }
        }
    }

    // Sort edge ascending order by weight
    sort(edges.begin(), edges.end());

    DisjointSet ds(size);
    vector<Edge> mstEdges;
    int mstCost = 0;

    for (size_t i = 0; i < edges.size(); i++)
    {
        Edge curEdge = edges[i];
        // If two vertices belong to different sets
        if (ds.find(curEdge.u) != ds.find(curEdge.v))
        {
            ds.merge(curEdge.u, curEdge.v); // merge
            mstEdges.push_back(curEdge);    // push MST
            mstCost += curEdge.weight;      // add edge summation
        }
    }

    // Check connectivity
    int rootCount = 0;
    for (int i = 0; i < size; i++)
    {
        if (ds.parent[i] == i)
            rootCount++;
    }

    if (rootCount > 1)
        return false;

    ofstream fout("log.txt", ios::app);
    fout << "========KRUSKAL========" << endl;

    // Converting to adjacency list form for output
    vector<vector<pair<int, int>>> resultAdj(size);

    for (size_t i = 0; i < mstEdges.size(); i++)
    {
        Edge e = mstEdges[i];
        resultAdj[e.u].push_back(make_pair(e.v, e.weight));
        resultAdj[e.v].push_back(make_pair(e.u, e.weight));
    }

    // output format
    for (int i = 0; i < size; i++)
    {
        fout << "[" << i << "]";
        sort(resultAdj[i].begin(), resultAdj[i].end());

        for (size_t j = 0; j < resultAdj[i].size(); j++)
        {
            pair<int, int> p = resultAdj[i][j];
            fout << " " << p.first << "(" << p.second << ")";
        }
        fout << endl;
    }
    fout << "Cost: " << mstCost << endl;
    fout << "=======================\n\n";
    fout.close();

    return true;
}

// Dijkstra, Use priority queue
bool Dijkstra(Graph *graph, char option, int vertex)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();
    if (vertex < 0 || vertex >= size)
        return false;

    // negative weight check
    for (int i = 0; i < size; i++)
    {
        map<int, int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(i, &adj);
        else
            graph->getAdjacentEdges(i, &adj);

        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            if (it->second < 0)
                return false; // Error when finding negative weights
        }
    }

    vector<int> dist(size, INF);
    vector<int> parent(size, -1);

    // min-heap
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[vertex] = 0;
    pq.push(make_pair(0, vertex));

    while (!pq.empty())
    {
        int cost = pq.top().first;
        int curr = pq.top().second;
        pq.pop();
        // Skip if you already visited on a shorter route
        if (dist[curr] < cost)
            continue;

        map<int, int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(curr, &adj);
        else
            graph->getAdjacentEdges(curr, &adj);

        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int next = it->first;
            int weight = it->second;
            // Renew if the distance is shorter
            if (dist[next] > cost + weight)
            {
                dist[next] = cost + weight;
                parent[next] = curr;
                pq.push(make_pair(dist[next], next));
            }
        }
    }

    ofstream fout("log.txt", ios::app);
    fout << "========DIJKSTRA========" << endl;
    if (option == 'O')
        fout << "Directed Graph Dijkstra" << endl;
    else
        fout << "Undirected Graph Dijkstra" << endl;

    fout << "Start: " << vertex << endl;

    for (int i = 0; i < size; i++)
    {
        fout << "[" << i << "] ";
        if (dist[i] == INF) // if Unreachable
        {
            fout << "x" << endl;
        }
        else
        {
            vector<int> path;
            int curr = i;
            while (curr != -1)
            {
                path.push_back(curr);
                curr = parent[curr];
            }
            // Route Reverse Output
            for (int k = (int)path.size() - 1; k >= 0; k--)
            {
                fout << path[k];
                if (k > 0)
                    fout << " -> ";
            }
            fout << " (" << dist[i] << ")" << endl;
        }
    }
    fout << "========================\n\n";
    fout.close();

    return true;
}
// Bellmanford
bool Bellmanford(Graph *graph, char option, int s_vertex, int e_vertex)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();
    if (s_vertex < 0 || s_vertex >= size || e_vertex < 0 || e_vertex >= size)
        return false;

    vector<int> dist(size, INF);
    vector<int> parent(size, -1);
    dist[s_vertex] = 0;

    // Number of vertices - 1 Repeat to update shortest distance
    for (int i = 0; i < size - 1; i++)
    {
        for (int u = 0; u < size; u++)
        {
            if (dist[u] == INF)
                continue;

            map<int, int> adj;
            if (option == 'O')
                graph->getAdjacentEdgesDirect(u, &adj);
            else
                graph->getAdjacentEdges(u, &adj);

            map<int, int>::iterator it;
            for (it = adj.begin(); it != adj.end(); it++)
            {
                int v = it->first;
                int w = it->second;

                if (dist[v] > dist[u] + w)
                {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                }
            }
        }
    }

    // Check negative cycle
    for (int u = 0; u < size; u++)
    {
        if (dist[u] == INF)
            continue;

        map<int, int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(u, &adj);
        else
            graph->getAdjacentEdges(u, &adj);

        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int v = it->first;
            int w = it->second;

            if (dist[v] > dist[u] + w)
            {
                return false; // Negative cycle still present if updated
            }
        }
    }

    ofstream fout("log.txt", ios::app);
    fout << "========BELLMANFORD========" << endl;
    if (option == 'O')
        fout << "Directed Graph Bellman-Ford" << endl;
    else
        fout << "Undirected Graph Bellman-Ford" << endl;

    if (dist[e_vertex] == INF)
    {
        fout << "x" << endl;
    }
    else
    {
        vector<int> path;
        int curr = e_vertex;
        while (curr != -1)
        {
            path.push_back(curr);
            curr = parent[curr];
        }

        for (int k = (int)path.size() - 1; k >= 0; k--)
        {
            fout << path[k];
            if (k > 0)
                fout << " -> ";
        }
        fout << "\nCost: " << dist[e_vertex] << endl;
    }
    fout << "===========================\n\n";
    fout.close();

    return true;
}
// Floyd
bool FLOYD(Graph *graph, char option)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();

    // Initialize 2D Array
    vector<vector<int>> dist(size, vector<int>(size, INF));

    for (int i = 0; i < size; i++)
        dist[i][i] = 0; // The diagonal part is zero

    // Enter initial edge information
    for (int i = 0; i < size; i++)
    {
        map<int, int> adj;
        if (option == 'O')
            graph->getAdjacentEdgesDirect(i, &adj);
        else
            graph->getAdjacentEdges(i, &adj);

        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int v = it->first;
            int w = it->second;
            // Select the smallest weight
            if (dist[i][v] > w)
                dist[i][v] = w;
        }
    }

    // Shortest distance update via triple iteration
    for (int k = 0; k < size; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (dist[i][k] != INF && dist[k][j] != INF)
                {
                    if (dist[i][j] > dist[i][k] + dist[k][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }

    // Check negative cycle
    for (int i = 0; i < size; i++)
    {
        if (dist[i][i] < 0)
            return false;
    }

    ofstream fout("log.txt", ios::app);
    fout << "========FLOYD========" << endl;
    if (option == 'O')
        fout << "Directed Graph Floyd" << endl;
    else
        fout << "Undirected Graph Floyd" << endl;

    fout << "\t";
    for (int i = 0; i < size; i++)
        fout << "[" << i << "]" << "\t";
    fout << endl;

    for (int i = 0; i < size; i++)
    {
        fout << "[" << i << "]" << "\t";
        for (int j = 0; j < size; j++)
        {
            if (dist[i][j] == INF)
                fout << "x" << "\t";
            else
                fout << dist[i][j] << "\t";
        }
        fout << endl;
    }
    fout << "=====================\n\n";
    fout.close();

    return true;
}
// Centrality
bool Centrality(Graph *graph)
{
    if (graph == NULL)
        return false;
    int size = graph->getSize();

    // Floyd logic reuse
    vector<vector<int>> dist(size, vector<int>(size, INF));
    for (int i = 0; i < size; i++)
        dist[i][i] = 0;

    for (int i = 0; i < size; i++)
    {
        map<int, int> adj;
        graph->getAdjacentEdges(i, &adj); // Centrality is based on a undirected graph

        map<int, int>::iterator it;
        for (it = adj.begin(); it != adj.end(); it++)
        {
            int v = it->first;
            int w = it->second;
            if (dist[i][v] > w)
                dist[i][v] = w;
        }
    }

    for (int k = 0; k < size; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (dist[i][k] != INF && dist[k][j] != INF)
                {
                    if (dist[i][j] > dist[i][k] + dist[k][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }

    for (int i = 0; i < size; i++)
        if (dist[i][i] < 0)
            return false;

    // centrality calculation
    vector<pair<double, int>> closeness(size);
    double maxVal = -1.0;

    for (int i = 0; i < size; i++)
    {
        int sumPath = 0;
        bool disconnected = false;

        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;
            if (dist[i][j] == INF)
            {
                disconnected = true;
                break;
            }
            sumPath += dist[i][j];
        }
        // Process if there are unconnected vertices or if the path sum is zero
        if (disconnected || sumPath == 0)
        {
            closeness[i] = make_pair(0.0, i);
        }
        else
        {
            //(N-1) / (Total distance to all vertices)
            double val = (double)(size - 1) / sumPath;
            closeness[i] = make_pair(val, i);
        }

        if (closeness[i].first > maxVal)
            maxVal = closeness[i].first;
    }

    ofstream fout("log.txt", ios::app);
    fout << "========CENTRALITY========" << endl;

    for (int i = 0; i < size; i++)
    {
        fout << "[" << i << "] ";
        if (closeness[i].first == 0.0)
        {
            fout << "x" << endl;
        }
        else
        {
            int sumPath = 0;
            for (int j = 0; j < size; j++)
            {
                if (i != j)
                    sumPath += dist[i][j];
            }

            fout << (size - 1) << "/" << sumPath;

            if (abs(closeness[i].first - maxVal) < 1e-9)
            {
                fout << " <- Most Central";
            }
            fout << endl;
        }
    }

    fout << "==========================\n\n";
    fout.close();
    return true;
}