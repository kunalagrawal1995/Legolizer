#include "node.hpp"

Node::Node() {
}

Node::Node(int x, int y, int z) {
	units.insert(Vector3(x,y,z));
}

Node::Node(Vector3 pos) {
	units.insert(pos);
}

int Node::number_intersections(Node* node) {
	int count = 0;

	set<Vector2> s1 = project();
	set<Vector2> s2 = node->project();

	set<Vector2>::iterator it = s1.begin();
	for(; it != s1.end(); it++) {
		if(s2.find(*it) != s2.end()) {
			count++;
		}
	}
	return count;
}

// set<Vector3>::iterator units_begin(){
// 	return units.begin();
// }


// set<Vector3>::iterator units_end(){
// 	return units.end();
// }

void Node::add_units(Node * node){
	units.insert(node->units.begin(), node->units.end());
}

void Node::add_connected(Node * node){
	map<Node*, double>::iterator iter;
	iter = node->connected.begin();
	for(; iter!= node->connected.end(); iter++) {
		int connections = this->number_intersections(iter->first);
		if(connections > 0)
			this->connected[iter->first] = connections;
	}
}

void Node::add_neighbours(Node * node1){
	neighbours.insert(node1->neighbours.begin(), node1->neighbours.end());
}

set<Vector2> Node::project() {
	set<Vector2> projection;

	set<Vector3>::iterator it = units.begin();
	for(; it != units.end(); it++) {
		projection.insert(Vector2((*it)[0], (*it)[1]));
	}
	return projection;
}

bool Node::check_neighbour(Node * n){
	for(auto unit_it : units){
		for(int i = 0; i < 4; i++){
			Vector3 v(unit_it);
			switch(i){
				case 0: v[0]--; break;
				case 1: v[0]++; break;
				case 2: v[1]++; break;
				case 3: v[1]--; break;
			}
			if(n->contains(v))
				return true;
		}
	}
	return false;
}

void Node::print(){
	for (auto iterator : units){
		cout << iterator << " ";
	}
	cout << endl;
}