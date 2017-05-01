#include "node.hpp"
#include "DGP/Graphics/Shader.hpp"

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

void Node::add_units(Node * node){
	units.insert(node->units.begin(), node->units.end());
}

void Node::add_children(Node * node){
	auto iter = node->children.begin();
	for(; iter!= node->children.end(); iter++) {
		int connections = this->number_intersections(*iter);
		if(connections > 0)
			this->children.insert(*iter);
	}
}

void Node::add_neighbours(Node * node1){
	neighbours.insert(node1->neighbours.begin(), node1->neighbours.end());
}

void Node::add_parents(Node * node){
	parents.insert(node->parents.begin(), node->parents.end());
}


set<Vector2> Node::project() {
	// cout << "project called" << endl; 
	set<Vector2> projection;
	int count = 0;
	// set<Vector3>::iterator it = units.begin();
	for(auto it : units) {
		// cout << count++ << " " << units.size() << endl;
		if(count > units.size())
			break;
		projection.insert(Vector2((it)[0], (it)[1]));
	}
	// cout << "project finished" << endl; 

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

AxisAlignedBox3 Node::getAABB() {
	return aabb;
}

ColorRGB Node::get_color() const{
	double vol = aabb.volume();
	if(vol == 1) {
		return ColorRGB(255, 255, 255);
	}
	else if (vol == 2) {
		return ColorRGB(255, 0, 0);
	}
	else if (vol == 4) {
		// 2 cases
		if((aabb.getExtent())[0] == 2 || (aabb.getExtent())[1] == 2)
			return ColorRGB(0, 255, 0);
		return ColorRGB(0, 0, 255);
	}
	else if (vol == 3) {
		return ColorRGB(255, 255, 0);
	}
	else if (vol == 6) {
		if((aabb.getExtent())[0] == 2 || (aabb.getExtent())[1] == 3 ||
			(aabb.getExtent())[1] == 2 || (aabb.getExtent())[0] == 3)
			return ColorRGB(0, 255, 255);
		return ColorRGB(255, 0, 255);
	}
	else if (vol == 8) {
		if((aabb.getExtent())[0] == 2 || (aabb.getExtent())[1] == 4 ||
			(aabb.getExtent())[1] == 2 || (aabb.getExtent())[0] == 4)
			return ColorRGB(100, 10, 200);
		return ColorRGB(10, 100, 205);
	}
	else if (vol == 12) {
		return ColorRGB(100, 201, 10);
	}
	else {
		return ColorRGB(10, 200, 255);
	}
}

void Node::draw(Graphics::RenderSystem & rs, int dimension, float scale, Vector3 trans, bool show_graph) const
{
  // // Make this static to ensure just one shader is created. Assumes rendersystem is constant, not the best design pattern.

	if(show_graph) {
		rs.setPointSize(5.0f);

		rs.beginPrimitive(Graphics::RenderSystem::Primitive::POINTS);
		rs.setColor(ColorRGB(255, 255, 255));
		Vector3 centroid = Vector3(0,0,0);
		for(auto pos: units) {

			centroid += Vector3(pos[0]+0.5, pos[1]+0.5, pos[2]+0.5)/(dimension * scale) -
							trans;
		}

		centroid /= units.size();
		rs.sendVertex(centroid);
		rs.endPrimitive();

		rs.setColor(ColorRGB(255, 255, 255));
		rs.pushShader();
		rs.setShader(NULL);
		
		rs.beginPrimitive(Graphics::RenderSystem::Primitive::LINES);
		for (auto child: children) {
			rs.sendVertex(centroid);
			rs.sendVertex((child->get_centroid())/(dimension*scale) - trans);
		}
		rs.endPrimitive();

		rs.popShader();		
		return;
	}

	rs.setPointSize(2.0f);

	rs.beginPrimitive(Graphics::RenderSystem::Primitive::QUADS);

	rs.setColor(get_color());
	
	for(auto pos: units) {

		Vector3 v0 = Vector3(pos[0], pos[1], pos[2])/(dimension * scale) -
						trans;

		Vector3 v1 = Vector3(pos[0], pos[1], pos[2]+1)/(dimension * scale) -
						trans;

		Vector3 v2 = Vector3(pos[0], pos[1]+1, pos[2]+1)/(dimension * scale) -
						trans;

		Vector3 v3 = Vector3(pos[0], pos[1]+1, pos[2])/(dimension * scale) -
						trans;

		Vector3 v4 = Vector3(pos[0]+1, pos[1], pos[2])/(dimension * scale) -
						trans;

		Vector3 v5 = Vector3(pos[0]+1, pos[1], pos[2]+1)/(dimension * scale) -
						trans;

		Vector3 v6 = Vector3(pos[0]+1, pos[1]+1, pos[2]+1)/(dimension * scale) -
						trans;

		Vector3 v7 = Vector3(pos[0]+1, pos[1]+1, pos[2])/(dimension * scale) -
						trans;

		rs.setNormal(Vector3(-1,0,0));
		rs.sendVertex(Vector3(v0));
		rs.sendVertex(Vector3(v1));
		rs.sendVertex(Vector3(v2));
		rs.sendVertex(Vector3(v3));

		rs.setNormal(Vector3(1,0,0));
		rs.sendVertex(Vector3(v4));
		rs.sendVertex(Vector3(v5));
		rs.sendVertex(Vector3(v6));
		rs.sendVertex(Vector3(v7));

		rs.setNormal(Vector3(0,-1,0));
		rs.sendVertex(Vector3(v0));
		rs.sendVertex(Vector3(v4));
		rs.sendVertex(Vector3(v7));
		rs.sendVertex(Vector3(v3));

		rs.setNormal(Vector3(0,1,0));
		rs.sendVertex(Vector3(v1));
		rs.sendVertex(Vector3(v5));
		rs.sendVertex(Vector3(v6));
		rs.sendVertex(Vector3(v2));

		rs.setNormal(Vector3(0,0,1));
		rs.sendVertex(Vector3(v0));
		rs.sendVertex(Vector3(v4));
		rs.sendVertex(Vector3(v5));
		rs.sendVertex(Vector3(v1));

		rs.setNormal(Vector3(0,0,-1));
		rs.sendVertex(Vector3(v3));
		rs.sendVertex(Vector3(v7));
		rs.sendVertex(Vector3(v6));
		rs.sendVertex(Vector3(v2));
	}

	rs.endPrimitive();

	rs.setColor(ColorRGB(0, 0, 0));

	rs.pushShader();
	rs.setShader(NULL);
	rs.beginPrimitive(Graphics::RenderSystem::Primitive::LINES);
	
	Vector3 p, q;
	for (int i = 0; i < 12; ++i) {
		aabb.getEdge(i, p, q);
		rs.sendVertex(p/(dimension*scale) - trans);
		rs.sendVertex(q/(dimension*scale) - trans);
	}
	rs.endPrimitive();

	rs.popShader();
}

void Node::recomputeAABB() {
	aabb.setNull();
	for(auto pos: units) {
		Vector3 v0 = Vector3(pos[0], pos[1], pos[2]);
		Vector3 v1 = Vector3(pos[0], pos[1], pos[2]+1);
		Vector3 v2 = Vector3(pos[0], pos[1]+1, pos[2]+1);
		Vector3 v3 = Vector3(pos[0], pos[1]+1, pos[2]);
		Vector3 v4 = Vector3(pos[0]+1, pos[1], pos[2]);
		Vector3 v5 = Vector3(pos[0]+1, pos[1], pos[2]+1);
		Vector3 v6 = Vector3(pos[0]+1, pos[1]+1, pos[2]+1);
		Vector3 v7 = Vector3(pos[0]+1, pos[1]+1, pos[2]);
		aabb.addPoint(v0);
		aabb.addPoint(v1);
		aabb.addPoint(v2);
		aabb.addPoint(v3);
		aabb.addPoint(v4);
		aabb.addPoint(v5);
		aabb.addPoint(v6);
		aabb.addPoint(v7);
	}
}


void Node::print(){
	for (auto iterator : units){
		cout << iterator << " ";
	}
	cout << endl;
}

Vector3 Node::get_centroid() {
	Vector3 centroid = Vector3(0,0,0);
	for(auto pos: units) {
		centroid += Vector3(pos[0]+0.5, pos[1]+0.5, pos[2]+0.5);
	}

	centroid /= units.size();
	return centroid;
}
