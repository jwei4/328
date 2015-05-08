#ifndef _Cloth_H
#define _Cloth_H

#include "Particle.h"
#include "Constraints.h"
#include <vector>

class Cloth {
private:
	int num_particles_width;				// num of particles in cloth width
	int num_particles_height;				// num of particles in cloth height

	int neightbor;

	std::vector<Particle> particles;		// vector of particles in cloth
	std::vector<Constraints> constraints;	// vecotr of constraints between particles in cloth

	Particle* getParticle(int x, int y);
	Particle* getTopP(int x, int y);
	Particle* getBotP(int x, int y);
	Particle* getLeftP(int x, int y);
	Particle* getRightP(int x, int y);
	int getVecIndex(int x, int y, int z);
	void makeConstraint(Particle *p1, Particle *p2);
	void drawLine(int index1, int index2);
	Vec3 calcLineNormal(Particle *p1,Particle *p2);
public:
	Cloth(){};
	~Cloth(){};
	Cloth(float width, float height);
	Cloth(float width, float height, int num_particles_width, int num_particles_height);
	void drawWire();
	void timeStep();
	Vec3 calcClothNormal(Particle* p);
	void addForce(const Vec3 direction);
	void transitionModel(Particle &p);
	void checkTearDistance(Particle* p);
	void checkRipDistance(Particle* p);	
	void checkRipDistance2(Particle* p);
	void ballCollision(const Vec3 center, const float radius);
	void ballCollision2(const Vec3 center, const float radius);
};

#endif