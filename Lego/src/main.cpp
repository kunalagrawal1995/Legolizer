#include "graph.hpp"
#include "legoBricks.hpp"
#include "Viewer.hpp"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]){
	// READ voxel input 

	Graph graph;
	graph.graph_init(argv[1]);
	cout << "GRAPH READ" << endl;
	read_lego_bricks("src/legoBricksConfig.txt");
	

	// graph.merge();
	set<Node*> temp = graph.find_articulation_points();
	cout<<"PRINTING ARTICULATION POINTS.."<<endl;
	for(auto iter: temp){
		iter->print();
	}
	cout<<"DONE"<<endl;
	// graph.print();
	Viewer viewer;
	viewer.setObject(&graph);
	viewer.launch(argc, argv);	

}