#include "Manager.h"
#include "GraphMethod.h"
#include "ListGraph.h"
#include "MatrixGraph.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// Structure to hold parsed command data
struct CommandData
{
	string command;
	string filename;
	char option;
	int vertex;		// start node
	int destVertex; // destination  node
	bool isValid;	// parameter is approptiate

	CommandData() : command(""), filename(""), option('\0'), vertex(-1), destVertex(-1), isValid(true) {}
};

// Parses a raw string line into CommandData structure
CommandData CommandParsing(string line)
{
	CommandData data;

	if (!line.empty() && line.back() == '\r')
	{
		line.pop_back();
	}

	stringstream ss(line); // command.txt file divided by line
	ss >> data.command;

	string extraArg; // Unnecessary parameter recognition

	// Distinguish the number of factors by command
	if (data.command == "LOAD")
	{
		// need 1 filename parameter
		ss >> data.filename;
		if (ss.fail() || (ss >> extraArg))
			data.isValid = false; // inappropriate command
	}
	else if (data.command == "BFS" || data.command == "DFS" || data.command == "DIJKSTRA")
	{
		// need 2 parameter
		ss >> data.option >> data.vertex;
		if (ss.fail() || (ss >> extraArg))
			data.isValid = false; // inappropriate command
	}
	else if (data.command == "BELLMANFORD")
	{
		// need 3 parameter
		ss >> data.option >> data.vertex >> data.destVertex;
		if (ss.fail() || (ss >> extraArg))
			data.isValid = false; // inappropriate command
	}
	else if (data.command == "FLOYD")
	{
		// need 1 option parameter
		ss >> data.option;
		if (ss.fail() || (ss >> extraArg))
			data.isValid = false; // inappropriate command
	}
	else if (data.command == "PRINT" || data.command == "KRUSKAL" || data.command == "CENTRALITY")
	{
		if (ss >> extraArg)
			data.isValid = false; // inappropriate command
	}
	return data;
}

Manager::Manager()
{
	graph = nullptr;
	load = 0;
}

Manager::~Manager()
{
	if (load && graph != nullptr) // It's been loaded, and if graph data exists
		delete graph;
}

void Manager::run(const char *command_txt)
{
	ofstream clearFout("log.txt", ios::trunc);
	clearFout.close();

	ifstream fin;
	fin.open(command_txt, ios_base::in);

	if (!fin)
	{
		fout.open("log.txt", ios::app);
		fout << "command file open error" << endl;
		fout.close();
		return;
	}

	string line;
	while (getline(fin, line))
	{
		if (line.empty())
			continue;

		CommandData cmdData = CommandParsing(line);
		if (cmdData.command.empty())
			continue;
		// LOAD
		if (cmdData.command == "LOAD")
		{
			if (!cmdData.isValid || cmdData.filename.empty() || !LOAD(cmdData.filename.c_str()))
				printErrorCode(100);
		}
		// PRINT
		else if (cmdData.command == "PRINT")
		{
			if (!cmdData.isValid || !PRINT())
				printErrorCode(200);
		}
		// BFS
		else if (cmdData.command == "BFS")
		{
			if (!cmdData.isValid || cmdData.option == '\0' || cmdData.vertex == -1 || !mBFS(cmdData.option, cmdData.vertex))
				printErrorCode(300);
		}
		// DFS
		else if (cmdData.command == "DFS")
		{
			if (!cmdData.isValid || cmdData.option == '\0' || cmdData.vertex == -1 || !mDFS(cmdData.option, cmdData.vertex))
				printErrorCode(400);
		}
		// KRUSKAL
		else if (cmdData.command == "KRUSKAL")
		{
			if (!cmdData.isValid || !mKRUSKAL())
				printErrorCode(500);
		}
		// DIJKSTRA
		else if (cmdData.command == "DIJKSTRA")
		{
			if (!cmdData.isValid || cmdData.option == '\0' || cmdData.vertex == -1 || !mDIJKSTRA(cmdData.option, cmdData.vertex))
				printErrorCode(600);
		}
		// BELLMANFORD
		else if (cmdData.command == "BELLMANFORD")
		{
			if (!cmdData.isValid || cmdData.option == '\0' || cmdData.vertex == -1 || cmdData.destVertex == -1 || !mBELLMANFORD(cmdData.option, cmdData.vertex, cmdData.destVertex))
				printErrorCode(700);
		}
		// FLOYD
		else if (cmdData.command == "FLOYD")
		{
			if (!cmdData.isValid || cmdData.option == '\0' || !mFLOYD(cmdData.option))
				printErrorCode(800);
		}
		// CENTRALITY
		else if (cmdData.command == "CENTRALITY")
		{
			if (!cmdData.isValid || !mCentrality())
				printErrorCode(900);
		}
		// EXIT
		else if (cmdData.command == "EXIT")
		{
			fout.open("log.txt", ios::app);
			fout << "========EXIT========" << endl;
			fout << "Success" << endl;
			fout << "====================\n\n";
			fout.close();
			break;
		}
	}
	fin.close();
}

// LOAD
bool Manager::LOAD(const char *filename)
{
	ifstream gFile(filename); // file open
	if (!gFile.is_open())
	{
		return false;
	}

	if (load && graph != nullptr)
	{
		delete graph;
		graph = nullptr;
		load = 0;
	}

	string typeStr; // L or M
	int size;		// node count

	gFile >> typeStr >> size;

	if (typeStr == "L")
		graph = new ListGraph(false, size); // Adjacent List
	else if (typeStr == "M")
		graph = new MatrixGraph(false, size); // Adjacent Matrix
	else
	{
		gFile.close();
		return false;
	}

	if (typeStr == "L") // Adjacent List
	{
		string line;
		getline(gFile, line);

		int currentVertex = -1; // start node

		while (getline(gFile, line))
		{
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			if (line.empty())
				continue;
			// parsing number in the line
			stringstream ss(line);
			vector<int> numbers;
			int num;
			while (ss >> num)
				numbers.push_back(num);

			if (numbers.empty())
				continue;
			// if size == 1 -> start node
			if (numbers.size() == 1)
			{
				currentVertex = numbers[0];
			}
			// if size != 1 -> start - destination
			else
			{
				// start node is appropriate?
				if (currentVertex != -1)
				{
					// inserting edge of twin node(start, destination)
					for (size_t i = 0; i < numbers.size(); i += 2)
					{
						if (i + 1 < numbers.size())
						{
							graph->insertEdge(currentVertex, numbers[i], numbers[i + 1]);
						}
					}
				}
			}
		}
	}
	else // Matrix
	{
		// Create a two-dimensional array by the number of nodes
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				int w;
				gFile >> w;
				// Inserts as connected edges only if the weight is not zero.
				if (w != 0)
					graph->insertEdge(i, j, w);
			}
		}
	}

	gFile.close();
	load = 1;

	fout.open("log.txt", ios::app);
	// Success output
	fout << "========LOAD========" << endl;
	fout << "Success" << endl;
	fout << "====================\n\n";
	fout.close();

	return true;
}

// print
bool Manager::PRINT()
{
	if (!graph)
		return false;

	fout.open("log.txt", ios::app);
	bool result = graph->printGraph(&fout);
	fout.close();

	return result;
}
// BFS
bool Manager::mBFS(char option, int vertex)
{
	if (!graph) // if no data
		return false;
	return BFS(graph, option, vertex);
}
// DFS
bool Manager::mDFS(char option, int vertex)
{
	if (!graph) // if no data
		return false;
	return DFS(graph, option, vertex);
}
// Dijkstra
bool Manager::mDIJKSTRA(char option, int vertex)
{
	if (!graph) // if no data
		return false;
	return Dijkstra(graph, option, vertex);
}
// Kruskal
bool Manager::mKRUSKAL()
{
	if (!graph) // if no data
		return false;
	return Kruskal(graph);
}
// Bellmanford
bool Manager::mBELLMANFORD(char option, int s_vertex, int e_vertex)
{
	if (!graph) // if no data
		return false;
	return Bellmanford(graph, option, s_vertex, e_vertex);
}
// Floyd
bool Manager::mFLOYD(char option)
{
	if (!graph) // if no data
		return false;
	return FLOYD(graph, option);
}

// Centrality
bool Manager::mCentrality()
{
	if (!graph) // if no data
		return false;
	return Centrality(graph);
}

// ERROR code
void Manager::printErrorCode(int n)
{
	fout.open("log.txt", ios::app);
	fout << "========ERROR=======" << endl;
	fout << n << endl;
	fout << "====================\n\n";
	fout.close();
}