#include "Vec3.h"
#include "Particle.h"
#include "Constraints.h"

Constraints::Constraints(Particle *p1, Particle *p2) :  p1(p1),p2(p2) {
	Vec3 vec = p1->getPos()-p2->getPos();
	rest_distance = vec.length();
}

void Constraints::satisfyConstraint() {
	Vec3 p1_to_p2 = p2->getPos()-p1->getPos(); // p1 -> p2 vector
	float current_distance = p1_to_p2.length(); 
	Vec3 correctionVector = p1_to_p2*(1 - rest_distance/current_distance); // The offset vector that could move p1 rest_distance of p2
	Vec3 correctionVectorHalf = correctionVector*0.5; // half it so that we can move BOTH p1 and p2 at the same time
	p1->offsetPos(correctionVectorHalf); // move p1 towards p2
	p2->offsetPos(-correctionVectorHalf); // move p2 the other way
}

