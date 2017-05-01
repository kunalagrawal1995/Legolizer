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
	cout << "LEGO BRICKS READ" << endl;

	graph.merge();
	set<Node*> temp = graph.find_articulation_points();
	cout << "PRINTING ARTICULATION POINTS.." << endl;
	for(auto iter: temp){
		iter->print();
	}
	
	if(temp.size() > 0){
		graph.remove_articulation_point(*(temp.begin()));
	}
	cout << "ARTICULATION POINTS REMOVAL ATTEMPTED" << endl;
	
	set<Node*> temp1 = graph.find_articulation_points();
	cout << "PRINTING ARTICULATION POINTS.." << endl;
	for(auto iter: temp1){
		iter->print();
	}
	cout << "DONE" << endl;
	// graph.print();
	Viewer viewer;
	viewer.setObject(&graph);
	viewer.launch(argc, argv);	

}