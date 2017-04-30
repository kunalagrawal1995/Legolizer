#ifndef __LEGOBRICKS_hpp__
#define __LEGOBRICKS_hpp__

#include <iostream>
#include "node.hpp"
#include "DGP/Vector2.hpp"
using namespace std;


void read_lego_bricks(string file_name);

bool check_brick_validity(Node * node1, Node* node2);

#endif