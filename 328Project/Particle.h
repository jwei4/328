#pragma once

#include <vector>
using std::vector;

class Particle {
private:
	bool movable; // make parts of cloth not movable
	bool gone;
	int warp; //up-down
	int weft; //left-right
	bool noConstraints;
	int top;
	int left;
	int right;
	int bottom;
	float mass; 
	Vec3 orig_pos;
	Vec3 pos; // current position 
	Vec3 old_pos; // position before timestep
	Vec3 acceleration; 
	Vec3 accumulated_normal; // used for OpenGL soft shading
public:
	Particle(Vec3 pos);
	Particle(){};
	~Particle(){};
	void addForce(Vec3 f);
	void timeStep();
	Vec3& getPos();
	void resetAcceleration();
	void offsetPos(const Vec3 v);
	void makeUnmovable();
	void addToNormal(Vec3 normal);
	Vec3& getNormal();
	void resetNormal();
	int getWarp();
	int getWeft();
	void setWarpHigh();
	void setWeftHigh();
	void setWarpLow();
	void setWeftLow();
	void setDisconnectedTop();
	bool getGone();
	void setGone();
	Vec3 getOrigPos();
	void setTop(int index);
	void setBottom(int index);
	void setLeft(int index);
	void setRight(int index);
	int getTop();
	int getBottom();
	int getLeft();
	int getRight();
	void setNoConstraints();
	bool getNoConstraints();
	bool getMovable();
};

