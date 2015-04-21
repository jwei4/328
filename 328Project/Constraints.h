#ifndef Constraints_H
#define Constraints_H

class Constraints {
private:
	float rest_distance; // distance between two particles
public:
	Particle *p1, *p2; // the two particles that are connected through this constraint
	Constraints();
	~Constraints();
	Constraints(Particle *p1, Particle *p2);
	void satisfyConstraint();
};

#endif