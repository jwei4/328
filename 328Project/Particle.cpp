#include "Vec3.h"
#include "Project.h"
#include "Particle.h"



	Particle::Particle(Vec3 pos) : pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), movable(true), accumulated_normal(Vec3(0,0,0)){}
	Particle::Particle(){}
	
Particle::~Particle(){}

	void Particle::addForce(Vec3 f) {
		acceleration += f/mass;
	}

	//change positions and then stop acceleration
	void Particle::timeStep() {
		if(movable) {
			Vec3 temp = pos;
			pos = pos + (pos-old_pos)*(1.0-DAMPING) + acceleration*TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = Vec3(0,0,0); 
		}
	}

	Vec3& Particle::getPos() {
		return pos;
	}

	void Particle::resetAcceleration() {
		acceleration = Vec3(0,0,0);
	}

	void Particle::offsetPos(const Vec3 v) { 
		if(movable) 
			pos += v;
	}

	void Particle::makeUnmovable() {
		movable = false;
	}

	void Particle::addToNormal(Vec3 normal) {
		accumulated_normal += normal.normalized();
	}

	Vec3& Particle::getNormal() { 
		return accumulated_normal;
	}

	void Particle::resetNormal() {
		accumulated_normal = Vec3(0,0,0);
	}

