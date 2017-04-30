#include "legoBricks.hpp"
#include <fstream>

set<vector<Vector2>> lego_bricks;

void read_lego_bricks(string file_name){
	ifstream lego_config;
	lego_config.open(file_name);
	int l1,l2;
	while(lego_config>>l1>>l2){
		std::vector<Vector2> v;
		for (int i = 0; i < l1; ++i){
			for (int j = 0; j < l2; ++j){
				v.push_back(Vector2(i,j));
			}
		}
		lego_bricks.insert(v);
		if(l1 != l2){
			std::vector<Vector2> v;
			for (int i = 0; i < l2; ++i){
				for (int j = 0; j < l1; ++j){
					v.push_back(Vector2(i,j));
				}
			}
			lego_bricks.insert(v);
		}
	}

	// for(auto iter: lego_bricks){
	// 	for(int i = 0; i<iter.size(); i++){
	// 		cout<<iter[i][0]<<" "<<iter[i][1]<<endl;
	// 	}
	// 	cout <<endl;
	// } 
}


bool check_brick_validity(Node * node1, Node* node2){
	set<Vector2> node1_projection = node1->project();
	set<Vector2> node2_projection = node2->project();

	set<Vector2> projection = node1_projection;
	projection.insert(node2_projection.begin(), node2_projection.end());
	Vector2 translate = *(projection.begin());
	
	for(auto bricks_iter: lego_bricks){
		if(bricks_iter.size() == projection.size()){
			bool result = true;
			for(auto bricks : bricks_iter){
				if(projection.count(bricks+translate) == 0){
					result = false;
					break;
				}
			}
			if(result)
				return true;
		}
	}

	return false;
}
