#ifndef __NODE_hpp__
#define __NODE_hpp__

#include <bits/stdc++.h>
#include "DGP/Vector3.hpp"
#include "DGP/Vector2.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"
using namespace std;
using namespace DGP;

class Node {
	set<Vector3> units;	// set of unit voxels contained in this node
	set<Node*> neighbours; // set of adjacent nodes to this node
	set<Node*> parents; // set of parents in the layer above 

	map<Node*, double> connected; // set of nodes in the lower layer connected with this node mapped to the number of connections
	AxisAlignedBox3 aabb;
public:
	Node();
	Node(int x, int y, int z);
	Node(Vector3 pos);
	int size(){return units.size();}
	int number_intersections(Node* node); // number of intersections with another node when both AFTER projection
	set<Vector2> project(); // project the brick on the x-y plane
	set<Vector3>::iterator units_begin(){return units.begin();}
	set<Vector3>::iterator units_end(){return units.end();}
	set<Node *>::iterator neighbours_begin(){return neighbours.begin();}
	set<Node *>::iterator neighbours_end(){return neighbours.end();}
	set<Node *>::iterator parents_begin(){return parents.begin();}
	set<Node *>::iterator parents_end(){return parents.end();}
	map<Node*, double>::iterator connected_begin(){return connected.begin();}
	map<Node*, double>::iterator connected_end(){return connected.end();}
	void add_units(Node * node);
	void add_connected(Node * node);
	void add_single_to_connected(Node * node){connected[node] = number_intersections(node);}
	void delete_connected(Node * node){connected.erase(node);}
	void add_neighbours(Node * node);
	void add_parents(Node * node);
	void delete_parent(Node *node){parents.erase(node);}
	void delete_neighbour(Node * node){neighbours.erase(node);}
	void add_neighbour(Node * node){neighbours.insert(node);}
	bool check_neighbour(Node * n);
	bool contains(Vector3 v){return units.count(v) == 1;}
	AxisAlignedBox3 getAABB();
	void draw(Graphics::RenderSystem & rs, int dimension, float scale, Vector3 trans) const;
	void print();
	void recomputeAABB();
};

#endif