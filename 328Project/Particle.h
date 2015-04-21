#pragma once
class Particle {
private:
	bool movable; // make parts of cloth not movable

	float mass; 
	Vec3 pos; // current position 
	Vec3 old_pos; // position before timestep
	Vec3 acceleration; 
	Vec3 accumulated_normal; // used for OpenGL soft shading
public:
	Particle(Vec3 pos);
	Particle();
	~Particle();
	void addForce(Vec3 f);
	void timeStep();
	Vec3& getPos();
	void resetAcceleration();
	void offsetPos(const Vec3 v);
	void makeUnmovable();
	void addToNormal(Vec3 normal);
	Vec3& getNormal();
	void resetNormal();
};

