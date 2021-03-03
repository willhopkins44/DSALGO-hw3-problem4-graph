#include <iostream>
#include <vector>
#include <stack>

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

	header* head = nullptr;
	int size;
	vector<int> mark; // vector to track which nodes have been visited

	Graph(int numVertices, vector<vector<int>> sets) { // constructor
		header* currentHeader = nullptr;
		size = numVertices;
		mark.resize(size);
		mark.assign(size, 0); // initialize all nodes to marked as unvisited
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
		size = oldGraph.size;
		mark = oldGraph.mark;
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

	/*header* getHead() {
		return head;
	}

	int getSize() {
		return size;
	}*/

	void printHeaders() {
		header* currentHeader = head;
		while (currentHeader->nextHeader) {
			cout << currentHeader->value << endl;
			currentHeader = currentHeader->nextHeader;
		}
		cout << currentHeader->value << endl;
	}

	void printAdjacencyList() {
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

	void printAdjacencyMatrix() {
		int currentSize = size;
		vector<vector<int>> matrix (currentSize);
		for (int row = 0; row < matrix.size(); row++) { // initialize the empty adjacency matrix
			matrix.at(row).resize(currentSize); // resize each row to have a column for each vertex
			matrix.at(row).assign(currentSize, 0); // initialize to 0
		}

		header* currentHeader = head;
		node* currentNode = new node;
		while (currentHeader) { // generate the adjacency matrix
			currentNode = currentHeader->firstNode;
			while (currentNode) {
				matrix.at(currentHeader->value).at(currentNode->value) = 1; // mark the current node as 1 in the adjacency matrix
				currentNode = currentNode->next;
			}
			currentHeader = currentHeader->nextHeader;
		}

		for (int row = 0; row < matrix.size(); row++) { // print the finalized adjacency matrix
			for (int col = 0; col < matrix.at(row).size(); col++) {
				cout << matrix.at(row).at(col) << " ";
			}
			cout << endl;
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

	int numVertices = input.at(0);
	parsedInputData parsedInput{ numVertices, {} }; // numVertices assigned the first value of input
													// sets are assigned an empty vector
	for (int i = 1; i < input.size(); i++) { // fill the sets vector with sets
		if (input.at(i) == -1) { // end of input
			break;
		}
		if (input.at(i) > numVertices - 1 || input.at(i+1) > numVertices - 1) { // if a vertex is greater than the number of vertices (i.e. you have a set 1 2 4 and are missing 3)
			throw invalid_argument("Missing intermediate vertices. For example, you may have included vertices 0 1 2 4 but forgotten 3");
		}
		vector<int> set = { input.at(i), input.at(i + 1) }; // create the set
		parsedInput.sets.push_back(set); // add the set to the parsedInput object
		i++; // increment i again so that we start at the next set instead of the next value (which belongs to the current set)
	}
	/*if (numVertices != parsedInput.sets.size()) {
		throw invalid_argument("Mismatch between number of vertices and number of inputted vertices");
	}*/
	return parsedInput;
};

vector<int> DFS(Graph& g, int v) { // returns vector of vertices sorted in order of visited by DFS
	int size = g.size;
	
	stack <int> visitedStack; // stack to track DFS action
	vector<int> visitedNodes; // output vector of visited nodes

	//vector<int> mark (size); // vector to track which nodes have been visited
	//mark.assign(size, 0); // initialize all values to 0 (unvisited)

	if (!g.mark.at(v)) {
		g.mark.at(v) = 1; // v is marked as visited
		visitedNodes.push_back(v); // v is added to the vector of visited nodes
		visitedStack.push(v); // v is added to stack
	}
	Graph::header* head = g.head;
	while (visitedStack.size() > 0) { // while there are items in the stack
		v = visitedStack.top(); // pop the first item off the stack and store its value
		visitedStack.pop();

		// add all vertices adjacent to v to the stack
		Graph::header* currentHeader = head;
		while (currentHeader) {
			if (v == currentHeader->value) {
				Graph::node* currentNode = currentHeader->firstNode;
				while (currentNode) {
					int currentValue = currentNode->value;
					if (!g.mark.at(currentValue)) { // if the current value has not been visited
						g.mark.at(currentValue) = 1; // mark as visited
						visitedNodes.push_back(currentValue); // add to vector of visited nodes
						visitedStack.push(currentValue); // push to visited stack
					}
					currentNode = currentNode->next;
				}
			}
			currentHeader = currentHeader->nextHeader;
		}
	}

	return visitedNodes;
}

vector<vector<int>> Components(Graph g) {
	// returns a list of connected component sets (vector of vectors)

	vector<vector<int>> components;
	for (int v = 0; v < g.size; v++) {
		vector<int> component = DFS(g, v); // call DFS for each vertex
		if (component.size() > 0) { // if the connected component exists (has a size),
			components.push_back(component); // add it to the list of connected components
		}
	}

	g.mark.assign(g.size, 0); // re-mark all nodes in g as unvisited

	return components;
}

vector<int> readInput() {
	vector<int> input;

	int numVertices;
	cout << "Enter the number of vertices: ";
	cin >> numVertices;
	input.push_back(numVertices);

	int vertex = 0;
	cout << "Enter edges. Enter -1 to finalize input" << endl;
	cout << "If your number of vertices is larger than the number of vertices used in your edges, it will be assumed that the remaining vertices are isolated (no neighbors)" << endl << endl;

	bool left = true;
	while (vertex > -1) {
		if (left) {
			cout << "Left vertex of edge ( (LEFT, RIGHT) ) or -1: ";
			left = !left;
		}
		else {
			cout << "Right vertex of edge ( (LEFT, RIGHT) ): ";
			left = !left;
		}
		cin >> vertex;
		input.push_back(vertex);
	}

	return input;
}

int main() {

	vector<int> input = readInput();

	try {
		parsedInputData parsedInput = parseInput(input);

		Graph g(parsedInput.numVertices, parsedInput.sets);
		cout << endl << "Adjacency list:" << endl << endl;
		g.printAdjacencyList();

		vector<vector<int>> connectedComponents = Components(g);
		cout << endl << "Connected components:" << endl << endl;
		for (int i = 0; i < connectedComponents.size(); i++) { // display all connected components
			cout << "{";
			for (int j = 0; j < connectedComponents.at(i).size(); j++) {
				if (j != 0) {
					cout << ",";
				}
				cout << " ";
				cout << connectedComponents.at(i).at(j);
			}
			cout << " }" << endl;
		}

		cout << endl << "Adjacency matrix:" << endl << endl;
		g.printAdjacencyMatrix();
	}
	catch (invalid_argument e) {
		cout << e.what() << endl;
	}

	return 0;
}