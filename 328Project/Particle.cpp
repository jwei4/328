#include "Vec3.h"
#include "Project.h"
#include "Particle.h"
#include <iostream>



Particle::Particle(Vec3 pos) : orig_pos(pos), pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), 
	movable(true), accumulated_normal(Vec3(0,0,0)), gone(false), top(-1), left(-1), right(-1), bottom(-1){
	 memset(disconnected, false, sizeof(bool)*4);
}

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

bool Particle::getDisconnectedLeft(){
	return disconnected[0];
}

bool Particle::getDisconnectedRight(){
	return disconnected[1];
}

bool Particle::getDisconnectedBottom(){
	return disconnected[2];
}

bool Particle::getDisconnectedTop(){
	return disconnected[3];
}

void Particle::setDisconnectedLeft(){
	disconnected[0] = true;
}

void Particle::setDisconnectedRight(){
	disconnected[1] = true;
}

void Particle::setDisconnectedBottom(){
	disconnected[2] = true;
}

void Particle::setDisconnectedTop(){
	disconnected[3] = true;
}

bool Particle::getGone(){
	return gone;
}

void Particle::setGone(){
	gone = true;
}

Vec3 Particle::getOrigPos(){
	return orig_pos;
}