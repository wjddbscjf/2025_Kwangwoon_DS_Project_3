#include "MatrixGraph.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

// Constructor - Graph Initialization
MatrixGraph::MatrixGraph(bool type, int size) : Graph(type, size)
{
	m_Mat = new int *[size];
	for (int i = 0; i < size; i++)
	{
		m_Mat[i] = new int[size];
		for (int j = 0; j < size; j++)
		{
			m_Mat[i][j] = 0;
		}
	}
}

// destructor
MatrixGraph::~MatrixGraph()
{
	if (m_Mat != nullptr)
	{
		for (int i = 0; i < m_Size; i++)
		{
			delete[] m_Mat[i];
		}
		delete[] m_Mat;
	}
}

// undirected perspective
void MatrixGraph::getAdjacentEdges(int vertex, map<int, int> *m)
{
	if (vertex < 0 || vertex >= m_Size)
		return;

	for (int i = 0; i < m_Size; i++)
	{
		// row, in-degree
		if (m_Mat[vertex][i] != 0)
		{
			m->insert(make_pair(i, m_Mat[vertex][i]));
		}
		// column, out-degree
		if (m_Mat[i][vertex] != 0)
		{
			m->insert(make_pair(i, m_Mat[i][vertex]));
		}
	}
}
// Directed perspective
void MatrixGraph::getAdjacentEdgesDirect(int vertex, map<int, int> *m)
{
	if (vertex < 0 || vertex >= m_Size)
		return;

	for (int i = 0; i < m_Size; i++)
	{
		if (m_Mat[vertex][i] != 0)
		{
			m->insert(make_pair(i, m_Mat[vertex][i]));
		}
	}
}
// Insert edge
void MatrixGraph::insertEdge(int from, int to, int weight)
{
	if (from >= 0 && from < m_Size && to >= 0 && to < m_Size)
	{
		m_Mat[from][to] = weight;
	}
}

// print graph, matrix perspective
bool MatrixGraph::printGraph(ofstream *fout)
{
	if (m_Size < 0)
		return false;

	*fout << "========PRINT========" << endl;

	*fout << "\t\t";
	for (int i = 0; i < m_Size; i++)
	{
		*fout << "[" << i << "]" << "\t";
	}
	*fout << endl;

	// output matrix format
	for (int i = 0; i < m_Size; i++)
	{
		*fout << "[" << i << "]" << "\t"; // row number
		for (int j = 0; j < m_Size; j++)
		{
			*fout << m_Mat[i][j] << "\t";
		}
		*fout << endl;
	}
	*fout << "=====================\n\n";
	return true;
}