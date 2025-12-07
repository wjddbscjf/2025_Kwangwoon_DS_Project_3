#include "ListGraph.h"
#include <iostream>
#include <utility>

using namespace std;

// Constructor - Graph Initialization
ListGraph::ListGraph(bool type, int size) : Graph(type, size)
{
    m_List = new map<int, int>[size];
}

// destructor
ListGraph::~ListGraph()
{
    delete[] m_List;
}

// undirected perspective
void ListGraph::getAdjacentEdges(int vertex, map<int, int> *m)
{
    for (auto const &item : m_List[vertex])
    {
        m->insert(make_pair(item.first, item.second));
    }

    for (int i = 0; i < m_Size; i++)
    {
        if (i == vertex)
            continue;                     // Except for yourself
        auto it = m_List[i].find(vertex); ////check an edge from vertex i to vertex
        if (it != m_List[i].end())
        {
            m->insert(make_pair(i, it->second)); // Add as neighbor
        }
    }
}

// Directed perspective
void ListGraph::getAdjacentEdgesDirect(int vertex, map<int, int> *m)
{
    for (auto const &item : m_List[vertex])
    {
        m->insert(make_pair(item.first, item.second));
    }
}
// Insert edge
void ListGraph::insertEdge(int from, int to, int weight)
{
    m_List[from].insert(make_pair(to, weight));
}
// print graph, List perspective
bool ListGraph::printGraph(ofstream *fout)
{
    if (m_Size < 0)
        return false;

    *fout << "========PRINT========" << endl;
    for (int i = 0; i < m_Size; i++)
    {
        *fout << "[" << i << "]";
        // no connected edge
        if (m_List[i].empty())
        {
            *fout << "->" << endl;
        }
        // connected edge
        else
        {
            for (auto const &item : m_List[i])
            {
                *fout << "->(" << item.first << "," << item.second << ")";
            }
            *fout << endl;
        }
    }
    *fout << "=====================\n\n";
    return true;
}