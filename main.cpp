#include <iostream>
#include <vector>
#include <stack>
//#define NULL -1 // because 0 is a valid vertex
// maybe don't do this? does it invalidate checking null as true/false?

using namespace std;

class Graph {
public:
	struct node {
		int value;
		node* next;
	};

	struct header {
		int value;
		header* nextHeader; // rename to next?
		node* firstNode;
	};

private:
	header* head = nullptr;
	int size;

public:
	Graph(int numVertices, vector<vector<int>> sets) { // constructor
		header* currentHeader = nullptr;
		size = numVertices;
		if (numVertices > 0) { // else, we don't create the headers because there are no vertices
			head = new header{ 0 };
			currentHeader = head;
		}
		for (int i = 1; i < numVertices; i++) { // doesn't execute if numVertices <= 0
			header* newHeader = new header{ i }; // create a new header in memory and point to it with newHeader
			currentHeader->nextHeader = newHeader; // attach head of graph to this new header
			currentHeader = newHeader; // update head to be the next header and loop until all vertices have been accounted for
		}

		for (int i = 0; i < sets.size(); i++) { // for each set
			int leftVertex = sets.at(i).at(0); // left vertex in the set (LEFT, RIGHT)
			int rightVertex = sets.at(i).at(1); // right vertex in the set (LEFT, RIGHT)
			currentHeader = head;
			node* currentNode = new node;
			while (currentHeader) { // traverse to headers containing these vertices
				if (currentHeader->value == leftVertex) {
					if (!currentHeader->firstNode) { // initialize a first node for the header
						currentHeader->firstNode = new node{ rightVertex, nullptr };
					}
					else {
						currentNode = currentHeader->firstNode;
						while (currentNode->next) { // traverse to the end of the header's nodes
							currentNode = currentNode->next;
						}
						node* newNode = new node{ rightVertex };
						currentNode->next = newNode; // add the right vertex to the adjacency list of the left vertex (LEFT: RIGHT RIGHT RIGHT etc.) (3: 2 1 4)
					}
				}
				else if (currentHeader->value == rightVertex) {
					if (!currentHeader->firstNode) { // initialize a first node for the header
						currentHeader->firstNode = new node{ leftVertex, nullptr };
					}
					else {
						currentNode = currentHeader->firstNode;
						while (currentNode->next) { // traverse to the end of the header's nodes
							currentNode = currentNode->next;
						}
						node* newNode = new node{ leftVertex };
						currentNode->next = newNode; // add the left vertex to the adjacency list for the right vertex (RIGHT: LEFT LEFT LEFT etc.) (3: 2 1 4)
					}
				}
				currentHeader = currentHeader->nextHeader; // go to the next header
			}
		}
	}

	Graph(header* in_head, int in_size) { // secondary constructor (do I need this?)
		head = in_head;
		size = in_size;
	}

	Graph(const Graph& oldGraph) { // copy constructor
		cout << "inside copy constructor" << endl;
		size = oldGraph.size;
		header* currentHeaderNew = nullptr;
		if (size > 0) {
			head = new header{ 0 };
			currentHeaderNew = head;
		}
		header* currentHeaderOld = oldGraph.head;
		while (currentHeaderOld) {
			currentHeaderNew->value = currentHeaderOld->value;
			node* currentNodeOld = currentHeaderOld->firstNode;
			node* currentNodeNew = nullptr;
			if (currentNodeOld) { // if the current header has nodes
				currentNodeNew = new node{ NULL, nullptr };
				currentHeaderNew->firstNode = currentNodeNew;
			}
			while (currentNodeOld) {
				currentNodeNew->value = currentNodeOld->value;
				currentNodeOld = currentNodeOld->next;
				if (currentNodeOld) { // if there is a next node
					node* newNode = new node{ NULL, nullptr }; // create a new node
					currentNodeNew->next = newNode; // point to it in the new graph
					currentNodeNew = currentNodeNew->next;
				}
			}
			currentHeaderOld = currentHeaderOld->nextHeader;
			if (currentHeaderOld) {
				header* newHeader = new header{ NULL, nullptr, nullptr };
				currentHeaderNew->nextHeader = newHeader;
				currentHeaderNew = currentHeaderNew->nextHeader;
			}
		}
	}

	~Graph() { // destructor
		header* currentHeader = head;
		while (currentHeader) {
			node* currentNode = currentHeader->firstNode;
			while (currentNode) {
				node* previousNode = currentNode;
				currentNode = currentNode->next;
				delete previousNode;
			}
			header* previousHeader = currentHeader;
			currentHeader = currentHeader->nextHeader;
			delete previousHeader;
		}
		head = nullptr;
	}

	header* getHead() {
		return head;
	}

	int getSize() {
		return size;
	}

	void printHeaders() {
		header* currentHeader = head;
		while (currentHeader->nextHeader) {
			cout << currentHeader->value << endl;
			currentHeader = currentHeader->nextHeader;
		}
		cout << currentHeader->value << endl;
	}

	void print() {
		header* currentHeader = head;
		node* currentNode = new node;
		while (currentHeader) {
			cout << currentHeader->value << ":";
			currentNode = currentHeader->firstNode;
			while (currentNode) {
				cout << currentNode->value << " ";
				currentNode = currentNode->next;
			}
			cout << endl;
			currentHeader = currentHeader->nextHeader;
		}
	}
};

struct parsedInputData {
	int numVertices;
	vector<vector<int>> sets;
};

parsedInputData parseInput(vector<int> input) { // parse the input and return a number of vertices and a vector of sets
	if (input.size() % 2 != 0) {
		throw invalid_argument("Incomplete set"); // input is odd, so there will be one incomplete set
	}

	parsedInputData parsedInput{ input.at(0), {} }; // numVertices assigned the first value of input
													// sets are assigned an empty vector
	for (int i = 1; i < sizeof(input); i++) { // fill the sets vector with sets
		if (input.at(i) == -1) { // end of input
			break;
		}
		vector<int> set = { input.at(i), input.at(i + 1) }; // create the set
		parsedInput.sets.push_back(set); // add the set to the parsedInput object
		i++; // increment i again so that we start at the next set instead of the next value (which belongs to the current set)
	}
	return parsedInput;
};

vector<int> DFS(Graph g, int v) { // returns vector of vertices sorted in order of visited by DFS
	int size = g.getSize();
	
	stack <int> visitedStack; // stack to track DFS action
	vector<int> visitedNodes; // output vector of visited nodes

	vector<int> mark (size); // vector to track which nodes have been visited
	mark.assign(size, 0); // initialize all values to 0 (unvisited)

	mark.at(v) = 1; // v is marked as visited
	visitedNodes.push_back(v); // v is added to the vector of visited nodes
	visitedStack.push(v); // v is added to stack
	Graph::header* head = g.getHead();
	while (visitedStack.size() > 0) { // while there are items in the stack
		v = visitedStack.top();
		visitedStack.pop();

		// add all vertices adjacent to v to stack
		Graph::header* currentHeader = head;
		while (currentHeader) {
			if (v == head->value) {
				Graph::node* currentNode = currentHeader->firstNode;
				while (currentNode) {
					int currentValue = currentNode->value;
					if (!mark.at(currentValue)) { // if the current value has not been visited
						mark.at(currentValue) = 1;
						visitedNodes.push_back(currentValue);
						visitedStack.push(currentValue);
					}
					currentNode = currentNode->next;
				}
			}
			currentHeader = currentHeader->nextHeader;
		}
	}

	return visitedNodes;
}

int main() {

	// function to get input
	
	vector<int> input = { 5, 0, 1, 1, 4, 2, 3, 1, 3, 3, 4, -1 };

	try {
		parsedInputData parsedInput = parseInput(input);
		/*cout << parsedInput.numVertices << endl;
		for (int i = 0; i < parsedInput.sets.size(); i++) {
			cout << parsedInput.sets.at(i).at(0) << "," << parsedInput.sets.at(i).at(1) << endl;
		}*/

		Graph g(parsedInput.numVertices, parsedInput.sets);
		g.print();
		vector<int> visitedNodes = DFS(g, 0);
		for (int i = 0; i < visitedNodes.size(); i++) {
			cout << visitedNodes.at(i) << " ";
		}
		cout << endl;
		//Graph* g = new Graph(parsedInput.numVertices, parsedInput.sets);
		//delete g;
	}
	catch (invalid_argument e) {
		cout << e.what() << endl;
	}

	return 0;
}