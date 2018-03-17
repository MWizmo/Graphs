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
public:
	virtual void readGraph(istream & ist) = 0;
	virtual void addEdge(int from, int to, int weight = 1) = 0;
	virtual void removeEdge(int from, int to) = 0;
	virtual int changeEdge(int from, int to, int newWeight) = 0;
	virtual vector<vector<int>> transformToAdjMatrix() = 0;
	virtual void transformToAdjList() = 0;
	virtual void transformToListOfEdges() = 0;
	virtual void writeGraph(string fileName) = 0;
};

class AdjMatrixGraph :public GraphRepresentationType {
private:
	int vertexNumber;
	bool isWeighed;
	bool isOriented;
	vector<vector<int>> adjacencyMatrix;
public:

	AdjMatrixGraph() {

	}

	AdjMatrixGraph(vector<vector<int>> matrix) {
		adjacencyMatrix = matrix;
		vertexNumber = matrix.size();
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
	}

	void removeEdge(int from, int to) override {
		adjacencyMatrix[to][from] = 0;
		if (!isOriented)
			adjacencyMatrix[from][to] = 0;
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

	void transformToAdjList() override {
		//return adjacencyMatrix;
	}

	void transformToListOfEdges() override {

	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		for (int i = 0; i < vertexNumber; i++) {
			for (int j = 0; j < vertexNumber; j++)
				file << adjacencyMatrix[i][j] << " ";
			file << endl;
		}
	}
};

class AdjListGraph :public GraphRepresentationType {
private:
	int vertexNumber;
	bool isWeighed;
	bool isOriented;
	vector<set<int>> unweighedAdjList;
	vector<set<pair<int, int>>> weighedAdjList;
public:
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
				}
				weighedAdjList.push_back(adjanceciesOfVertex);
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				getline(ist, currentVertexList);
				vector<string> neighbours = Split(currentVertexList, ' ');
				set<int> adjanceciesOfVertex;
				for (int j = 0; j < neighbours.size(); j++) {
					int adjancecy = stoi(neighbours[j]);
					adjanceciesOfVertex.insert(adjancecy);
				}
				unweighedAdjList.push_back(adjanceciesOfVertex);
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			weighedAdjList[from - 1].insert(pair<int, int>(to, weight));
			if (!isOriented)
				weighedAdjList[to - 1].insert(pair<int, int>(from, weight));
		}
		else {
			unweighedAdjList[from - 1].insert(to);
			if (!isOriented)
				unweighedAdjList[to - 1].insert(from);
		}

	}

	void removeEdge(int from, int to) override {
		if (isWeighed) {
			for (auto iter = weighedAdjList[from - 1].begin(); iter != weighedAdjList[from - 1].end(); iter++)
				if ((iter->first) == to) {
					weighedAdjList[from - 1].erase(iter);
					break;
				}
			if (!isOriented) {
				for (auto iter = weighedAdjList[to - 1].begin(); iter != weighedAdjList[to - 1].end(); iter++)
					if ((iter->first) == from) {
						weighedAdjList[to - 1].erase(iter);
						break;
					}
			}
		}
		else {
			unweighedAdjList[from - 1].erase(to);
			if (!isOriented)
				unweighedAdjList[to - 1].erase(from);
		}
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (auto iter = weighedAdjList[from - 1].begin(); iter != weighedAdjList[from - 1].end(); iter++)
			if ((iter->first) == to) {
				weight = iter->second;
				break;
			}
		this->removeEdge(from, to);
		this->addEdge(from, to, newWeight);
		if (!isOriented) {
			this->removeEdge(to, from);
			this->addEdge(to, from, newWeight);
		}
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
				for (auto iter = weighedAdjList[i].begin(); iter != weighedAdjList[i].end(); iter++) {
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
				for (auto iter = weighedAdjList[i].begin(); iter != weighedAdjList[i].end(); iter++) {
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

	void transformToAdjList() override {

	}

	void transformToListOfEdges() override {

	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				file << i + 1 << ": ";
				for (auto iter = weighedAdjList[i].begin(); iter != weighedAdjList[i].end(); iter++)
					file << iter->first << "(" << iter->second << ") ";
				file << endl;
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				file << i + 1 << ": ";
				for (auto iter = unweighedAdjList[i].begin(); iter != unweighedAdjList[i].end(); iter++)
					file << *iter << " ";
				file << endl;
			}
		}
		file.close();
	}
};

class ListOfEdgesGraph :public GraphRepresentationType {
private:
	int vertexNumber;
	int edgesNumber;
	bool isWeighed;
	bool isOriented;
	vector<pair<int, int>> unweighedEdgesList;
	vector <tuple<int, int, int>> weighedEdgesList;
public:
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
				weighedEdgesList.push_back(tuple<int, int, int>(stoi(vertexes[0]), stoi(vertexes[1]), stoi(vertexes[2])));
			}
		}
		else {
			for (int i = 0; i < edgesNumber; i++) {
				getline(ist, currentEdge);
				vector<string> vertexes = Split(currentEdge, ' ');
				unweighedEdgesList.push_back(pair<int, int>(stoi(vertexes[0]), stoi(vertexes[1])));
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			weighedEdgesList.push_back(tuple<int, int, int>(from, to, weight));
			if (!isOriented)
				weighedEdgesList.push_back(tuple<int, int, int>(to, from, weight));
		}
		else {
			unweighedEdgesList.push_back(pair<int, int>(from, to));
			if (!isOriented)
				unweighedEdgesList.push_back(pair<int, int>(to, from));
		}
	}

	void removeEdge(int from, int to) override {
		if (isWeighed) {
			for (int i = 0; i < edgesNumber; i++) {
				if ((get<0>(weighedEdgesList[i]) == from) && (get<1>(weighedEdgesList[i]) == to)) {
					weighedEdgesList.erase(weighedEdgesList.begin() + i);
					break;
				}
			}
			if (!isOriented) {
				for (int i = 0; i < edgesNumber; i++) {
					if ((get<0>(weighedEdgesList[i]) == to) && (get<1>(weighedEdgesList[i]) == from)) {
						weighedEdgesList.erase(weighedEdgesList.begin() + i);
						break;
					}
				}
			}
		}
		else {
			for (int i = 0; i < edgesNumber; i++) {
				if ((get<0>(unweighedEdgesList[i]) == from) && (get<1>(unweighedEdgesList[i]) == to)) {
					unweighedEdgesList.erase(unweighedEdgesList.begin() + i);
					break;
				}
			}
			if (!isOriented) {
				for (int i = 0; i < edgesNumber; i++) {
					if ((get<0>(unweighedEdgesList[i]) == to) && (get<1>(unweighedEdgesList[i]) == from)) {
						unweighedEdgesList.erase(unweighedEdgesList.begin() + i);
						break;
					}
				}
			}
		}
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (int i = 0; i < edgesNumber; i++) {
			if ((get<0>(weighedEdgesList[i]) == from) && (get<1>(weighedEdgesList[i]) == to)) {
				weight = get<2>(weighedEdgesList[i]);
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

		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				adjMatrix[get<0>(weighedEdgesList[i]) - 1][get<1>(weighedEdgesList[i]) - 1] = get<2>(weighedEdgesList[i]);
				if (!isOriented)
					adjMatrix[get<1>(weighedEdgesList[i]) - 1][get<0>(weighedEdgesList[i]) - 1] = get<2>(weighedEdgesList[i]);
				else
					adjMatrix[get<1>(weighedEdgesList[i]) - 1][get<0>(weighedEdgesList[i]) - 1] = -get<2>(weighedEdgesList[i]);
			}
		}
		else
		{
			for (int i = 0; i < vertexNumber; i++) {
				adjMatrix[get<0>(weighedEdgesList[i]) - 1][get<1>(weighedEdgesList[i]) - 1] = 1;
				if (!isOriented)
					adjMatrix[get<1>(weighedEdgesList[i]) - 1][get<0>(weighedEdgesList[i]) - 1] = 1;
				else
					adjMatrix[get<1>(weighedEdgesList[i]) - 1][get<0>(weighedEdgesList[i]) - 1] = -1;
			}
		}
		return adjMatrix;
	}

	void transformToAdjList() override {

	}

	void transformToListOfEdges() override {

	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		if (isWeighed) {
			for (int i = 0; i < edgesNumber; i++)
				file << get<0>(weighedEdgesList[i]) << "-" << get<1>(weighedEdgesList[i]) << "(" << get<2>(weighedEdgesList[i]) << ")" << endl;
		}
		else {
			for (int i = 0; i < edgesNumber; i++)
				file << get<0>(unweighedEdgesList[i]) << "-" << get<1>(unweighedEdgesList[i]) << endl;
		}
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
		representation = new AdjMatrixGraph(representation->transformToAdjMatrix());
	}

	void transformToAdjList() {
		//vector<vector<int>> matrix=representation->transformToAdjList();
	}

	void transformToListOfEdges() {
		representation->transformToListOfEdges();
	}

	void writeGraph(string fileName) {
		representation->writeGraph(fileName);
	}
};

int main()
{
	Graph graph = Graph();
	graph.readGraph("input.txt");
	graph.transformToAdjMatrix();
	graph.writeGraph("output.txt");
	setlocale(LC_ALL, "rus");
	system("pause");
	return 0;
}
