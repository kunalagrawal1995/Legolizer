#include <bits/stdc++.h>
#ifndef __GRAPH_hpp__
#define __GRAPH_hpp__

#include "node.hpp"
#include "legoBricks.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"
using namespace std;

class Graph {
private:
	set<Node*> nodes;
	map<Vector3, Node*> unit_node_map;
	AxisAlignedBox3 aabb;

	Vector3 trans;
	float scale;
	int dimension;	// cell size = 1/dimension

	void articulation_helper(Node * root, 
		map<Node*, bool> &visited,
		map<Node*, int> &disc,
		map<Node*, int> &low,
		map<Node*, Node*> &parent,
		set<Node*> &articulation_points);

public:	
	Graph();

	//read voxel file and set up each node
	void graph_init(string voxel_file);
	// merges two given nodes and returns the merged node
	Node* merge_nodes(Node* node1, Node* node2);
	// split a node into 1*1 units
	set<Node *> split_node(Node* node);

	// merge till no possible merges left
	void merge();
	// merge till possible but the selection is only from the given set of nodes
	void merge(set<Node *> &nodes_subset);
	
	// check if the node can be merged with a neighbour, return pointer to the node if possible else NULL
	Node * check_merge(Node* node);
	// determines the cost for joining node1 and node2
	int merge_cost_fn(Node* node1, Node* node2);

	// returns the number of nodes in the graph
	int num_nodes(){return nodes.size();}
	int num_unit_nodes_map(){return unit_node_map.size();}


	set<Node*> find_articulation_points();
	void remove_articulation_point(Node * node);
	void render_articulation_points(Graphics::RenderSystem & rs);

	AxisAlignedBox3 getAABB();
	void draw(Graphics::RenderSystem & rs, bool show_graph) const;
	void print();
};
#endif