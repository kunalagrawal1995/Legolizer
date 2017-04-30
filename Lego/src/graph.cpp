#include "graph.hpp"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

//global variables
Vector3 trans;
float scale;
int dimension;

Graph::Graph(){
}

void Graph::merge_nodes(Node* node1, Node* node2) {

  Node* mergedNode = new Node();

  mergedNode->add_units(node1);
  mergedNode->add_units(node2);

  mergedNode->add_connected(node1);
  mergedNode->add_connected(node2);

  mergedNode->add_neighbours(node1);
  mergedNode->add_neighbours(node2);

  mergedNode->delete_neighbour(node1);
  mergedNode->delete_neighbour(node2);

  for (auto it = node1->neighbours_begin(); it != node1->neighbours_end(); ++it){
    (*it)->delete_neighbour(node1); 
    (*it)->add_neighbour(mergedNode); 
  }

  for (auto it = node2->neighbours_begin(); it != node2->neighbours_end(); ++it){
    (*it)->delete_neighbour(node2); 
    (*it)->add_neighbour(mergedNode); 
  }

  for(auto it = node1->units_begin(); it != node1->units_end(); ++it){
    unit_node_map[*it] = mergedNode;
  }

  for(auto it = node2->units_begin(); it != node2->units_end(); ++it){
    unit_node_map[*it] = mergedNode;
  }

  nodes.erase(node1);
  nodes.erase(node2);
  nodes.insert(mergedNode);
  delete(node1);
  delete(node2);
}

// ADD update of unit_node_map if needed
void Graph::split_nodes(Node* node){
  // this set can be used as needed
  set<Node *> unit_nodes;
  auto set_it = node->units_begin();
  auto set_end = node->units_end();
  for(; set_it != set_end; set_it++){
    Node* n = new Node(*set_it);
    n->add_connected(node);
  }

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
    (*it2)->delete_neighbour(node);
  }
}

void Graph::merge(){
  set<Node*> failed_merge_nodes;
  int count_failed_merges = 0;
  int size = nodes.size();
  while(true){    
    int pick = rand()%size;
    set<Node*>::iterator node_merge_choice = nodes.begin();
    for(int i = 1; i<pick; i++){
      node_merge_choice++;
    }

    // till a new choice for merge not found
    while(failed_merge_nodes.find(*node_merge_choice) != failed_merge_nodes.end()){
      pick = rand()%size;
      node_merge_choice = nodes.begin();
      for(int i = 1; i<pick; i++){
        node_merge_choice++;
      }
    }
    
    if(merge(*node_merge_choice)){
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

bool Graph::merge(Node* node){
  auto neighbours_iter = node->neighbours_begin();
  auto neighbours_end_iter = node->neighbours_end();
  int connections = 0;
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
  if(optimal != NULL){
    merge_nodes(node, optimal);
    return true;
  }
  else
    return false;
}

int Graph::merge_cost_fn(Node * node1, Node * node2){
  set<Node *> connected_bricks;
  for(auto iter = node1->connected_begin(); iter != node1->connected_end(); iter++){
    connected_bricks.insert(iter->first);
  }

  for(auto iter = node2->connected_begin(); iter != node2->connected_end(); iter++){
    connected_bricks.insert(iter->first);
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
    for(int y = 0; y < dimension; y++){
      for (int z = 0; z < dimension; ++z){
        for (int x = 0; x < dimension; ++x){
          voxel_file>>in;

          if(in){
            Node* node = new Node(x,y,z);
            if(z != 0 && unit_node_map.count(Vector3(x,y,z-1))){
              node->add_connected(unit_node_map[Vector3(x,y,z-1)]);
            }
            if(y != 0 && unit_node_map.count(Vector3(x,y-1,z))){
              node->add_neighbour(unit_node_map[Vector3(x,y-1,z)]);
              unit_node_map[Vector3(x,y-1,z)]->add_neighbour(node);
            }
            if(x != 0 && unit_node_map.count(Vector3(x-1,y,z))){
              node->add_neighbour(unit_node_map[Vector3(x-1,y,z)]);
              unit_node_map[Vector3(x-1,y,z)]->add_neighbour(node);
            }
            nodes.insert(node);
            unit_node_map.insert(make_pair(Vector3(x,y,z),node));
          }
        }
      }
    }
    voxel_file.close();
    std::cout<<"Read "<<nodes.size()<<" voxels from "<<voxel_filename<<std::endl;
  }
  else{
    std::cerr<<"Unable to open file "<<voxel_filename<<std::endl;
  }
}