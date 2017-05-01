#include "graph.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

Graph::Graph(){
}

void print_neighbors(Node * node){
  for (auto it = node->neighbours_begin(); it != node->neighbours_end(); ++it){
	cout << (*it) << " "; 
  }
  cout << endl;
}

Node* Graph::merge_nodes(Node* node1, Node* node2) {
  // std::cout<<"merging 2 nodes"<<std::endl;
	// 1: union of units
  // 2: union of children
  // 3: union of parents
  // 4: update parents of children
  // 5: update children of parents
  // 6: unions of neighbours(delete node1 and node2)
  // 7: update neighbours


	Node* mergedNode = new Node();

  cout << "1" << endl;
	mergedNode->add_units(node1);
	mergedNode->add_units(node2);

  cout << "2" << endl;
	mergedNode->add_children(node1);
	mergedNode->add_children(node2);

  cout << "5" << endl;
	for(auto it = node1->parents_begin(); it != node1->parents_end(); it++){
		(*it)->add_child(mergedNode);
		(*it)->delete_child(node1);
	}

	for(auto it = node2->parents_begin(); it != node2->parents_end(); it++){
		(*it)->add_child(mergedNode);
		(*it)->delete_child(node2);
	}

  cout << "4" << endl;
  for(auto it = node1->children_begin(); it != node1->children_end(); it++){
      (*it)->delete_parent(node1);
      (*it)->add_parent(mergedNode);
  }

  for(auto it = node2->children_begin(); it != node2->children_end(); it++){
      (*it)->delete_parent(node2);
      (*it)->add_parent(mergedNode);
  }

  cout << "3" << endl;
	mergedNode->add_parents(node1);
	mergedNode->add_parents(node2);

  cout << "6" << endl;
	mergedNode->add_neighbours(node1);
	mergedNode->add_neighbours(node2);
	mergedNode->delete_neighbour(node1);
	mergedNode->delete_neighbour(node2);

  cout << "7" << endl;
	node1->delete_neighbour(node2);
	node2->delete_neighbour(node1);

  cout << "8" << endl;
	for (auto it = node1->neighbours_begin(); it != node1->neighbours_end(); ++it){
		cout << (*it) << endl;
    (*it)->delete_neighbour(node1); 
		(*it)->add_neighbour(mergedNode); 
	}

  cout << "9" << endl;
	for (auto it = node2->neighbours_begin(); it != node2->neighbours_end(); ++it){
		(*it)->delete_neighbour(node2); 
		(*it)->add_neighbour(mergedNode); 
	}

  cout << "10" << endl;
  // updating unit to node mapping
	for(auto it = node1->units_begin(); it != node1->units_end(); ++it){
		unit_node_map[*it] = mergedNode;
	}

	for(auto it = node2->units_begin(); it != node2->units_end(); ++it){
		unit_node_map[*it] = mergedNode;
	}

  cout << "11" << endl;
	nodes.erase(node1);
	nodes.erase(node2);
	nodes.insert(mergedNode);
	mergedNode->recomputeAABB();
  cout << node1 << " " << node2 << " " << mergedNode << endl;
  return mergedNode;
	// shifted to individual segments to delete these
	// delete(node1);
	// delete(node2);
}

// ADD update of unit_node_map if needed
set<Node *> Graph::split_node(Node* node){
	// this set can be used as needed
  // 1. separate the units set
  // 2. update the parent of children
  // 3. add parent of all units
  // 4. update parent of the node
  // 5. update neighbours for the units

	set<Node *> unit_nodes;
	auto set_it = node->units_begin();
	auto set_end = node->units_end();
  // task 1
	for(; set_it != set_end; set_it++){
		Node* n = new Node(*set_it);
    unit_nodes.insert(n);
		n->recomputeAABB();
	}

  // task 5
	for(auto it1: unit_nodes){
		for (auto it2: unit_nodes){
			if((it1)->check_neighbour(it2))
				(it1)->add_neighbour(it2);
		}
	}

	for (auto it2 = node->neighbours_begin(); it2 != node->neighbours_end(); it2++){
		for(auto it1: unit_nodes){
			if((it1)->check_neighbour(*it2)){
				(it1)->add_neighbour(*it2);
				(*it2)->add_neighbour(it1);
			}
		}
  }

	for (auto it2 = node->neighbours_begin(); it2 != node->neighbours_end(); it2++){
    (*it2)->delete_neighbour(node);
  }

  // task 3 and 4
  for(auto it1 = node->parents_begin(); it1 != node->parents_end(); it1++){
    (*it1)->delete_child(node);
    for(auto it2 : unit_nodes){
      if((*it1)->number_intersections(it2) > 0){
        (*it1)->add_child(it2);
        it2->add_parent(*it1);
      }
    }
  }

  // task 2
  for(auto it1 = node->children_begin(); it1 != node->children_end(); it1++){
    (*it1)->delete_parent(node);
    for(auto it2 : unit_nodes){
      if((*it1)->number_intersections(it2) > 0){
        (*it1)->add_parent(it2);
        it2->add_child(*it1);
      }
    }
  }

  nodes.erase(node);
  nodes.insert(unit_nodes.begin(), unit_nodes.end());
  return unit_nodes;
}

void Graph::merge(){
  set<Node*> failed_merge_nodes;
  int count_failed_merges = 0;
  int size = nodes.size();
  while(true){    
  	int pick = rand()%size;
  	set<Node*>::iterator node_merge_choice = nodes.begin();
  	for(int i = 1; i<=pick; i++){
  	  node_merge_choice++;
  	}

  	// till a new choice for merge not found
  	while(failed_merge_nodes.find(*node_merge_choice) != failed_merge_nodes.end()){
  	  pick = rand()%size;
  	  node_merge_choice = nodes.begin();
  	  for(int i = 1; i<=pick; i++){
  		node_merge_choice++;
  	  }
  	}
  	
  	Node * optimal = check_merge(*node_merge_choice);
  	if(optimal != NULL){
      // delete no longer happend in the merge function
      merge_nodes(*node_merge_choice, optimal);
      // delete(*node_merge_choice);
      // delete(optimal);
      count_failed_merges = 0;
  	  failed_merge_nodes.clear();
  	}
  	else{
  	  count_failed_merges++;
  	  failed_merge_nodes.insert(*node_merge_choice);
  	}

  	size = nodes.size();
  	if(count_failed_merges == nodes.size()){
  	  break;
  	}
  }
}

void Graph::merge(set<Node*> &nodes_subset){
  set<Node*> failed_merge_nodes;
  int count_failed_merges = 0;
  int size = nodes_subset.size();
  while(true){    
    cout << size << " " << count_failed_merges << endl;
    int pick = rand()%size;
    set<Node*>::iterator node_merge_choice = nodes_subset.begin();
    for(int i = 1; i<=pick; i++){
      node_merge_choice++;
    }

    // till a new choice for merge not found
    while(failed_merge_nodes.find(*node_merge_choice) != failed_merge_nodes.end()){
      pick = rand()%size;
      node_merge_choice = nodes_subset.begin();
      for(int i = 1; i<=pick; i++){
        node_merge_choice++;
      }
    }
    
    cout << "node picked " << endl;
    Node * optimal = check_merge(*node_merge_choice);
    if(optimal != NULL){
      cout << "merging node found " << endl;
      cout << optimal << " " << *node_merge_choice << endl;
      Node * merged_node = merge_nodes(*node_merge_choice, optimal);
      // also update the nodes_subset;  the graph has already been updated in the merge_nodes function
      nodes_subset.erase(*node_merge_choice);
      if(nodes_subset.count(optimal) == 1)
        nodes_subset.erase(optimal);
      nodes_subset.insert(merged_node);
      // delete(*node_merge_choice);
      // delete(optimal);
      count_failed_merges = 0;
      failed_merge_nodes.clear();
    }
    else{
      cout << "merging node not found " << endl;
      count_failed_merges++;
      failed_merge_nodes.insert(*node_merge_choice);
    }

    size = nodes_subset.size();
    if(count_failed_merges == size){
      break;
    }
  }
  cout << "function ended" << endl;
}

Node* Graph::check_merge(Node* node){
  auto neighbours_iter = node->neighbours_begin();
  auto neighbours_end_iter = node->neighbours_end();
  int connections = -1;
  Node* optimal = NULL;
  for(; neighbours_iter != neighbours_end_iter; neighbours_iter++){
    if(check_brick_validity(node, *neighbours_iter)){
  	  int merge_cost = merge_cost_fn(node, *neighbours_iter);
  	  if(connections < merge_cost){
  		  connections = merge_cost;
  		  optimal = *neighbours_iter;
  	  }   
  	}
  }

  return optimal;
}

int Graph::merge_cost_fn(Node * node1, Node * node2){
	set<Node *> connected_bricks;
	for(auto iter = node1->children_begin(); iter != node1->children_end(); iter++){
		connected_bricks.insert(*iter);
	}

	for(auto iter = node2->children_begin(); iter != node2->children_end(); iter++){
		connected_bricks.insert(*iter);
	}

	return connected_bricks.size();
}

void Graph::graph_init(string voxel_filename){
	ifstream voxel_file;
	voxel_file.open(voxel_filename);
	if(voxel_file.is_open()){
		string line;
		//read header
		string keyword;
		while(getline(voxel_file,line)){
			istringstream ss(line);
			ss>>keyword;
			if(keyword == "dim"){
				ss>>dimension;
			}
			else if(keyword == "translate"){
				ss>>trans[0]>>trans[1]>>trans[2];
			}
			else if(keyword == "scale"){
				ss>>scale;
			}
			else if(keyword == "data"){
				voxel_file>>keyword;
				break;
			}
		}

		int in;
		for(int z = 0; z < dimension; z++){
			for (int x = 0; x < dimension; x++){
				for (int y = 0; y < dimension; y++){
					voxel_file>>in;

					if(in){
						Node* node = new Node(x,z,y);
						if(y != 0 && unit_node_map.count(Vector3(x,z,y-1))){
							node->add_child(unit_node_map[Vector3(x,z,y-1)]);
							unit_node_map[Vector3(x,z,y-1)]->add_parent(node);
						}
						if(z != 0 && unit_node_map.count(Vector3(x,z-1,y))){
							node->add_neighbour(unit_node_map[Vector3(x,z-1,y)]);
							unit_node_map[Vector3(x,z-1,y)]->add_neighbour(node);
						}
						if(x != 0 && unit_node_map.count(Vector3(x-1,z,y))){
							node->add_neighbour(unit_node_map[Vector3(x-1,z,y)]);
							unit_node_map[Vector3(x-1,z,y)]->add_neighbour(node);
						}
						nodes.insert(node);
						unit_node_map.insert(make_pair(Vector3(x,z,y),node));
						node->recomputeAABB();
					}
				}
			}
		}
		voxel_file.close();
		aabb.addPoint(-Vector3(trans[0], trans[1], trans[2]));
		aabb.addPoint(Vector3(  1.0/scale - trans[0],
								1.0/scale - trans[1],
								1.0/scale - trans[2]));
		std::cout<<"Read "<<nodes.size()<<" voxels from "<<voxel_filename<<std::endl;
	}
	else{
		std::cerr<<"Unable to open file "<<voxel_filename<<std::endl;
	}
}

void Graph::articulation_helper(Node * root, 
	map<Node*, bool> &visited,
	map<Node*, int> &disc,
	map<Node*, int> &low,
	map<Node*, Node*> &parent,
	set<Node*> &articulation_points){

	static int time = 0;
	int children = 0;

	visited[root] = true;

	disc[root] = ++time;
	low[root] = time;

	// looking at all the neighbours of root
	for(auto iter = root->children_begin(); iter != root->children_end(); iter++){
		if(!visited[(*iter)]){
			children++;
			parent[(*iter)] = root;
			articulation_helper((*iter), visited, disc, low, parent, articulation_points);
			low[root] = min(low[root], low[(*iter)]);

			// root is articulation point if 
			if(parent[root] == NULL && children > 1){
				articulation_points.insert(root);
			}
			if(parent[root] != NULL && low[(*iter)] >= disc[root]){
				articulation_points.insert(root);
			}
		}
		else if((*iter) != parent[root]){
			low[root] = min(low[root], disc[(*iter)]);
		}
	}

	for(auto iter = root->parents_begin(); iter != root->parents_end(); iter++){
		if(!visited[(*iter)]){
			children++;
			parent[(*iter)] = root;
			articulation_helper((*iter), visited, disc, low, parent, articulation_points);
			low[root] = min(low[root], low[(*iter)]);

			// root is articulation point if 
			if(parent[root] == NULL && children > 1){
				articulation_points.insert(root);
			}
			if(parent[root] != NULL && low[(*iter)] >= disc[root]){
				articulation_points.insert(root);
			}
		}
		else if((*iter) != parent[root]){
			low[root] = min(low[root], disc[(*iter)]);
		}
	}
}

set<Node*> Graph::find_articulation_points(){
	map<Node*, bool> visited;
	map<Node*, int> disc;
	map<Node*, int> low;
	map<Node*, Node*> parent;
	set<Node*> articulation_points;


	// initialize
	for(auto iter: nodes){
		visited[iter] = false;
		parent[iter] = NULL;
	}

	for(auto iter: nodes){
		if(visited[iter] == false){
			articulation_helper(iter, visited, disc, low, parent, articulation_points);
		}
	}
	return articulation_points;
}

void Graph::remove_articulation_point(Node * node){
  // 1. split the node and all the neighbours
  // 2. call the merge on the resulting set of unit nodes
  set<Node*> units = split_node(node);
  set<Node*> neighbours;
  neighbours.insert(node->neighbours_begin(), node->neighbours_end());
  for(auto iter : neighbours){
    set<Node*> neighbour_units = split_node(iter);
    units.insert(neighbour_units.begin(), neighbour_units.end());
  }

  cout << units.size() << endl;
  for(auto iter : neighbours){
    // delete(iter);
  }
  // delete(node);
  merge(units);

}

void Graph::render_articulation_points(Graphics::RenderSystem & rs){
	set<Node*> pts = find_articulation_points();
	rs.setPointSize(10.0f);
	rs.beginPrimitive(Graphics::RenderSystem::Primitive::POINTS);
    rs.setColor(ColorRGB(255, 0, 0));
    for(auto pt: pts) {
      Vector3 centroid = pt->get_centroid();
      rs.sendVertex(centroid/(dimension * scale) - trans);
    }
    rs.endPrimitive();
}

AxisAlignedBox3 Graph::getAABB() {
	return aabb;
}

void Graph::draw(Graphics::RenderSystem & rs, bool show_graph) const
{
	for(auto node: nodes) {
		node->draw(rs, dimension, scale, trans, show_graph);
	} 
}

void Graph::print(){
  for(auto iterator : nodes){
	iterator->print();
  }
}