#ifndef Cloth_H
#define Cloth_H

#include "Particle.h"
#include "Constraints.h"
#include <vector>

class Cloth {
private:
	int num_particles_width;				// num of particles in cloth width
	int num_particles_height;				// num of particles in cloth height

	std::vector<Particle> particles;		// vector of particles in cloth
	std::vector<Constraints> constraints;	// vecotr of constraints between particles in cloth

	Particle* getParticle(int x, int y);
	void makeConstraint(Particle *p1, Particle *p2);
	Vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3);
	void drawLine(Particle *p1, Particle *p2);
	Vec3 calcLineNormal(Particle *p1,Particle *p2);
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color);
public:
	Cloth(){};
	~Cloth(){};
	Cloth(float width, float height);
	Cloth(float width, float height, int num_particles_width, int num_particles_height);
	void drawWire();
	void drawShaded();
	void timeStep();
	void addForce(const Vec3 direction);
	void ballCollision(const Vec3 center, const float radius);
};

#endif