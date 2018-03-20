#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <tuple>
#include <string>
#include <iterator>
using namespace std;

vector<string> Split(string str, char delimiter) {
	vector<string> vec;
	string currentString = "";
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != delimiter)
			currentString.push_back(str[i]);
		else {
			if (currentString != "")
				vec.push_back(currentString);
			currentString = "";
		}
	}
	if (currentString != "")
		vec.push_back(currentString);
	return vec;
}

class GraphRepresentationType {
protected:
	int vertexNumber = 0;
	int edgesNumber = 0;
	bool isWeighed;
	bool isOriented;
public:
	virtual void readGraph(istream & ist) = 0;
	virtual void addEdge(int from, int to, int weight = 1) = 0;
	virtual void removeEdge(int from, int to) = 0;
	virtual int changeEdge(int from, int to, int newWeight) = 0;
	virtual vector<vector<int>> transformToAdjMatrix() = 0;
	virtual vector<set<pair<int, int>>> transformToAdjList() = 0;
	virtual vector <tuple<int, int, int>> transformToListOfEdges() = 0;
	virtual void writeGraph(string fileName) = 0;
	virtual tuple<bool, bool, int, int> GetInfo() = 0;
};

class AdjMatrixGraph :public GraphRepresentationType {
private:
	vector<vector<int>> adjacencyMatrix;
public:

	AdjMatrixGraph() {
		
	}

	AdjMatrixGraph(vector<vector<int>> matrix, tuple<bool, bool, int, int> info) {
		adjacencyMatrix = matrix;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
		edgesNumber = get<3>(info);
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> isOriented;
		ist >> isWeighed;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++) {
				int cell;
				ist >> cell;
				row.push_back(cell);
			}
			adjacencyMatrix.push_back(row);
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		adjacencyMatrix[to][from] = weight;
		if (!isOriented)
			adjacencyMatrix[from][to] = weight;
		else
			adjacencyMatrix[from][to] = -weight;
		edgesNumber++;
	}

	void removeEdge(int from, int to) override {
		adjacencyMatrix[to][from] = 0;
		adjacencyMatrix[from][to] = 0;
		edgesNumber--;
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight = adjacencyMatrix[from][to];
		adjacencyMatrix[to][from] = newWeight;
		if (!isOriented)
			adjacencyMatrix[from][to] = newWeight;
		return weight;
	}

	vector<vector<int>> transformToAdjMatrix() override {
		return adjacencyMatrix;
	}

	vector<set<pair<int, int>>> transformToAdjList() override {
		vector<set<pair<int, int>>> adjList;

		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		vector<tuple<int, int, int>> edgesList;

		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		for (int i = 0; i < vertexNumber; i++) {
			for (int j = 0; j < vertexNumber; j++)
				file << adjacencyMatrix[i][j] << " ";
			file << endl;
		}
	}

	tuple<bool, bool, int, int> GetInfo() override {
		return tuple<bool, bool, int, int>(isWeighed, isOriented, vertexNumber, edgesNumber);
	}
};

class AdjListGraph :public GraphRepresentationType {
private:
	vector<set<pair<int, int>>> adjList;
public:
	AdjListGraph() {

	}

	AdjListGraph(vector<set<pair<int, int>>> list, tuple<bool, bool, int, int> info) {
		adjList=list;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
		edgesNumber = get<3>(info);
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> isOriented;
		ist >> isWeighed;
		string currentVertexList;
		getline(ist, currentVertexList);

		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				getline(ist, currentVertexList);
				vector<string> neighbours = Split(currentVertexList, ' ');
				set<pair<int, int>> adjanceciesOfVertex;
				for (int j = 0; j < neighbours.size(); j += 2) {
					pair<int, int> adjancecy(stoi(neighbours[j]), stoi(neighbours[j + 1]));
					adjanceciesOfVertex.insert(adjancecy);
					edgesNumber++;
				}
				adjList.push_back(adjanceciesOfVertex);
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				getline(ist, currentVertexList);
				vector<string> neighbours = Split(currentVertexList, ' ');
				set<pair<int,int>> adjanceciesOfVertex;
				for (int j = 0; j < neighbours.size(); j++) {
					pair<int, int> adjancecy(stoi(neighbours[j]),1);
					adjanceciesOfVertex.insert(adjancecy);
					edgesNumber++;
				}
				adjList.push_back(adjanceciesOfVertex);
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			adjList[from - 1].insert(pair<int, int>(to, weight));
		}
		else {
			adjList[from - 1].insert(pair<int, int>(to, 1));
		}
		edgesNumber++;
	}

	void removeEdge(int from, int to) override {
			for (auto iter = adjList[from - 1].begin(); iter != adjList[from - 1].end(); iter++)
				if ((iter->first) == to) {
					adjList[from - 1].erase(iter);
					break;
				}
			edgesNumber--;
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (auto iter = adjList[from - 1].begin(); iter != adjList[from - 1].end(); iter++)
			if ((iter->first) == to) {
				weight = iter->second;
				break;
			}
		this->removeEdge(from, to);
		this->addEdge(from, to, newWeight);
		return weight;
	}
	
	vector<vector<int>> transformToAdjMatrix() override {
		vector<vector<int>> adjMatrix;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++)
				row.push_back(0);
			adjMatrix.push_back(row);
		}

		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
					adjMatrix[i][iter->first - 1] = iter->second;
					if (!isOriented)
						adjMatrix[iter->first - 1][i] = iter->second;
					else
						adjMatrix[iter->first - 1][i] = -(iter->second);
				}
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
					adjMatrix[i][iter->first - 1] = 1;
					if (!isOriented)
						adjMatrix[iter->first - 1][i] = 1;
					else
						adjMatrix[iter->first - 1][i] = -1;
				}
			}
		}
		return adjMatrix;
	}
	
	vector<set<pair<int, int>>> transformToAdjList() override {
		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		vector<tuple<int, int, int>> edgesList;
		for (int i = 0; i < vertexNumber; i++) {
			for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
				edgesList.push_back(tuple<int, int, int>(i + 1, iter->first, iter->second));
			}
		}
		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		for (int i = 0; i < vertexNumber; i++) {
			file << i + 1 << ": ";
			for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
				file << iter->first;
				if (isWeighed)
					file << "(" << iter->second << ") ";	
			}
			file << endl;
		}
		file.close();
	}

	tuple<bool, bool, int, int> GetInfo() override {
		return tuple<bool, bool, int, int>(isWeighed, isOriented, vertexNumber, edgesNumber);
	}
};

class ListOfEdgesGraph :public GraphRepresentationType {
private:
	vector <tuple<int, int, int>> edgesList;
public:
	ListOfEdgesGraph() {

	}

	ListOfEdgesGraph(vector <tuple<int, int, int>> list, tuple<bool, bool, int, int> info) {
		edgesList = list;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
		edgesNumber = get<3>(info);
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> edgesNumber;
		ist >> isOriented;
		ist >> isWeighed;
		string currentEdge;
		getline(ist, currentEdge);
		if (isWeighed) {
			for (int i = 0; i < edgesNumber; i++) {
				getline(ist, currentEdge);
				vector<string> vertexes = Split(currentEdge, ' ');
				edgesList.push_back(tuple<int, int, int>(stoi(vertexes[0]), stoi(vertexes[1]), stoi(vertexes[2])));
			}
		}
		else {
			for (int i = 0; i < edgesNumber; i++) {
				getline(ist, currentEdge);
				vector<string> vertexes = Split(currentEdge, ' ');
				edgesList.push_back(tuple<int, int, int>(stoi(vertexes[0]), stoi(vertexes[1]),1));
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			edgesList.push_back(tuple<int, int, int>(from, to, weight));
		}
		else {
			edgesList.push_back(tuple<int,int, int>(from, to,1));
		}
		edgesNumber++;
	}

	void removeEdge(int from, int to) override {
		for (int i = 0; i < edgesNumber; i++) {
			if ((get<0>(edgesList[i]) == from) && (get<1>(edgesList[i]) == to)) {
				edgesList.erase(edgesList.begin() + i);
				break;
			}
		}
		edgesNumber--;
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (int i = 0; i < edgesNumber; i++) {
			if ((get<0>(edgesList[i]) == from) && (get<1>(edgesList[i]) == to)) {
				weight = get<2>(edgesList[i]);
				break;
			}
		}
		this->removeEdge(from, to);
		this->addEdge(from, to, newWeight);
		return weight;
	}
	
	vector<vector<int>> transformToAdjMatrix() override {
		vector<vector<int>> adjMatrix;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++)
				row.push_back(0);
			adjMatrix.push_back(row);
		}

			for (int i = 0; i < vertexNumber; i++) {
				adjMatrix[get<0>(edgesList[i]) - 1][get<1>(edgesList[i]) - 1] = get<2>(edgesList[i]);
				if (!isOriented)
					adjMatrix[get<1>(edgesList[i]) - 1][get<0>(edgesList[i]) - 1] = get<2>(edgesList[i]);
				else
					adjMatrix[get<1>(edgesList[i]) - 1][get<0>(edgesList[i]) - 1] = -get<2>(edgesList[i]);
			}
		return adjMatrix;
	}

	vector<set<pair<int, int>>> transformToAdjList() override {
		vector<set<pair<int, int>>> adjList;
		for (int i = 0; i < vertexNumber; i++) {
			set<pair<int, int>> vertex;
			adjList.push_back(vertex);
		}

		for (int i = 0; i < edgesNumber; i++) {
			adjList[get<0>(edgesList[i])-1].insert(pair<int, int>(get<1>(edgesList[i]), get<2>(edgesList[i])));
		}
		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
			for (int i = 0; i < edgesNumber; i++) {
				file << get<0>(edgesList[i]) << "-" << get<1>(edgesList[i]);
				if (isWeighed)
					file << "(" << get<2>(edgesList[i]) << ")";
				file << endl;
			}
		file.close();
	}

	tuple<bool, bool, int, int> GetInfo() override {
		return tuple<bool, bool, int, int>(isWeighed, isOriented, vertexNumber, edgesNumber);
	}
};

class Graph {
private:
	GraphRepresentationType* representation = nullptr;
public:
	void readGraph(string fileName) {
		ifstream file(fileName);
		char repr;
		file >> repr;
		switch (repr)
		{
		case 'C':	representation = new AdjMatrixGraph(); break;
		case 'L':	representation = new AdjListGraph(); break;
		case 'E':	representation = new ListOfEdgesGraph(); break;
		}
		representation->readGraph(file);
		file.close();
	}

	void addEdge(int from, int to, int weight = 1) {
		representation->addEdge(from, to, weight);
	}

	void removeEdge(int from, int to) {
		representation->removeEdge(from, to);
	}

	int changeEdge(int from, int to, int newWeight) {
		return representation->changeEdge(from, to, newWeight);
	}

	void transformToAdjMatrix() {
		representation = new AdjMatrixGraph(representation->transformToAdjMatrix(),representation->GetInfo());
	}

	void transformToAdjList() {
		representation = new AdjListGraph(representation->transformToAdjList(), representation->GetInfo());
	}

	void transformToListOfEdges() {
		representation = new ListOfEdgesGraph(representation->transformToListOfEdges(), representation->GetInfo());
	}

	void writeGraph(string fileName) {
		representation->writeGraph(fileName);
	}
};

int main()
{
	Graph graph = Graph();
	graph.readGraph("input.txt");
	graph.transformToAdjList();
	graph.writeGraph("output.txt");
	setlocale(LC_ALL, "rus");
	system("pause");
	return 0;
}
