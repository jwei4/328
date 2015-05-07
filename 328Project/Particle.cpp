#include "Vec3.h"
#include "Project.h"
#include "Particle.h"
#include <iostream>

Particle::Particle(Vec3 pos) : orig_pos(pos), pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), 
	movable(true), accumulated_normal(Vec3(0,0,0)), gone(false), top(-1), left(-1), right(-1), bottom(-1), warp(0), weft(0){
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

int Particle::getWarp(){
	return warp;
}

int Particle::getWeft(){
	return weft;
}

int Particle::setWarpHigh(){
	warp = 2;
}

int Particle::setWeftHigh(){
	weft = 2;
}

int Particle::setWarpLow(){
	weft = 1;
}

int Particle::setWeftLow(){
	weft = 1;
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

void Particle::setLeft(int index){
	left = index;
}

void Particle::setRight(int index){
	right = index;
}

void Particle::setTop(int index){
	top = index;
}

void Particle::setBottom(int index){
	bottom = index;
}

int Particle::getTop(){
	return top;
}

int Particle::getBottom(){
	return bottom;
}

int Particle::getLeft(){
	return left;
}

int Particle::getRight(){
	return right;
}