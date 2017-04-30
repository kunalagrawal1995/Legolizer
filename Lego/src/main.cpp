#include "graph.hpp"
#include "legoBricks.hpp"
#include <iostream>
using namespace std;

int main(int argc, char const *argv[]){
	// READ voxel input 

	Graph graph;
	graph.graph_init(argv[1]);
	cout << "GRAPH READ" << endl;
	read_lego_bricks("src/legoBricksConfig.txt");
	cout << "READ LEGO BRICKS" << endl;
	graph.merge();
	graph.print();
}