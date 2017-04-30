#include "graph.hpp"
#include "legoBricks.hpp"
#include "Viewer.hpp"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]){
	// READ voxel input 

	Graph graph;
	graph.graph_init(argv[1]);
	read_lego_bricks("src/legoBricksConfig.txt");

	Viewer viewer;
	viewer.setObject(&graph);
	viewer.launch(argc, argv);
}