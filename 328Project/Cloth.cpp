#include "Vec3.h"
#include "Project.h"
#include "Cloth.h"
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <iostream>

Particle* Cloth::getParticle(int x, int y) {return &particles[y*num_particles_width + x];}
void Cloth::makeConstraint(Particle *p1, Particle *p2) {constraints.push_back(Constraints(p1,p2));}


Vec3 Cloth::calcLineNormal(Particle *p1,Particle *p2) {
	Vec3 pos1 = p1->getPos();
	Vec3 pos2 = p2->getPos();

	Vec3 v1 = pos2-pos1;

	return v1.cross(v1);
}

// calculate normal for forces
Vec3 Cloth::calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3) {
	Vec3 pos1 = p1->getPos();
	Vec3 pos2 = p2->getPos();
	Vec3 pos3 = p3->getPos();

	Vec3 v1 = pos2-pos1;
	Vec3 v2 = pos3-pos1;

	return v1.cross(v2);
}

void Cloth::drawLine(Particle *p1, Particle *p2){
	glNormal3fv((GLfloat *) &(p1->getNormal().normalized()));
	glVertex3fv((GLfloat *) &(p1->getPos() ));

	glNormal3fv((GLfloat *) &(p2->getNormal().normalized()));
	glVertex3fv((GLfloat *) &(p2->getPos() ));
}

//draw shaded triangle
void Cloth::drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color) {
	glColor3fv( (GLfloat*) &color );

	glNormal3fv((GLfloat *) &(p1->getNormal().normalized() ));
	glVertex3fv((GLfloat *) &(p1->getPos() ));

	glNormal3fv((GLfloat *) &(p2->getNormal().normalized() ));
	glVertex3fv((GLfloat *) &(p2->getPos() ));

	glNormal3fv((GLfloat *) &(p3->getNormal().normalized() ));
	glVertex3fv((GLfloat *) &(p3->getPos() ));
}

Cloth::Cloth(float width, float height) : num_particles_width(width/THICKNESS), num_particles_height(height/THICKNESS){
	std::cout << "width: " << num_particles_width << " height: " << num_particles_height;
	particles.resize(num_particles_width*num_particles_height); //set particle vector size to size of cloth

	// creating particles from (0,0,0) to (width,-height,0)
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			Vec3 pos = Vec3(width * (x/(float)num_particles_width),-height * (y/(float)num_particles_height),0);
			particles[y*num_particles_width+x]= Particle(pos); // insert into particle vector
		}
	}

	// Connecting neighbor particles with constraints 
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getParticle(x+1,y));
			if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x,y+1));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x+1,y),getParticle(x,y+1));
		}
	}

	// Connecting secondary neighbors with constraints
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			if (x<num_particles_width-2) makeConstraint(getParticle(x,y),getParticle(x+2,y));
			if (y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x,y+2));
			if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x+2,y+2));
			if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x+2,y),getParticle(x,y+2));			
		}
	}

	// set part of cloth unmovable (top left and right)
	//for(int i=0;i<num_particles_width; i++)
	for(int i = 0; i < 3; i++)
	{
		//top row
		getParticle(0+i ,0)->makeUnmovable(); 
		getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		//getParticle(0+i, num_particles_height-1)->makeUnmovable();
		//getParticle(num_particles_width-1-i, num_particles_height-1)->makeUnmovable();
	}
}

Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height){
	particles.resize(num_particles_width*num_particles_height); //set particle vector size to size of cloth

	// creating particles from (0,0,0) to (width,-height,0)
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			Vec3 pos = Vec3(width * (x/(float)num_particles_width),-height * (y/(float)num_particles_height),0);
			particles[y*num_particles_width+x]= Particle(pos); // insert into particle vector
		}
	}

	// Connecting neighbor particles with constraints 
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getParticle(x+1,y));
			if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x,y+1));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x+1,y),getParticle(x,y+1));
		}
	}

	// Connecting secondary neighbors with constraints
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			if (x<num_particles_width-2) makeConstraint(getParticle(x,y),getParticle(x+2,y));
			if (y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x,y+2));
			if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x+2,y+2));
			if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x+2,y),getParticle(x,y+2));			
		}
	}

	// set part of cloth unmovable (top left and right)
	for(int i=0;i<num_particles_width; i++)
	{
		getParticle(0+i ,0)->makeUnmovable(); 
		getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		getParticle(0+i, num_particles_height-1)->makeUnmovable();
		getParticle(num_particles_width-1-i, num_particles_height-1)->makeUnmovable();
	}
}

//draw Wire version
void Cloth::drawWire(){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++) {
		(*particle).resetNormal();
	}
	//use vectors with particles that keep track of neighbors, knn algorithm
	glBegin(GL_LINES);
	for(int x = 0; x < num_particles_width; x++) {
		for(int y=0; y < num_particles_height; y++) {
			if(x == num_particles_width-1 && y < num_particles_height-1){
				//std::cout << "x,y GDR: " << getParticle(x,y)->getDisconnectedRight()<<" x,y+1 GDT: " << getParticle(x,y+1)->getDisconnectedTop();
				if(!getParticle(x,y)->getDisconnectedRight() && !getParticle(x,y+1)->getDisconnectedTop())
				//if(!getParticle(x,y)->getGone() && !getParticle(x,y+1)->getGone())
					drawLine(getParticle(x,y), getParticle(x, y+1));
			}
			if(y == num_particles_height-1 && x < num_particles_width-1){
				if(!getParticle(x,y)->getDisconnectedBottom() && !getParticle(x+1, y)->getDisconnectedLeft())
				//if(!getParticle(x,y)->getGone() && !getParticle(x+1, y)->getGone())
					drawLine(getParticle(x,y), getParticle(x+1, y));
			}
			if(x < num_particles_width-1 && y < num_particles_height-1){
				if(!getParticle(x,y)->getDisconnectedRight() && !getParticle(x,y+1)->getDisconnectedTop())
				//if(!getParticle(x,y)->getGone() && !getParticle(x,y+1)->getGone())
					drawLine(getParticle(x,y), getParticle(x, y+1));
				if(!getParticle(x,y)->getDisconnectedBottom() && !getParticle(x+1, y)->getDisconnectedLeft())
				//if(!getParticle(x,y)->getGone() && !getParticle(x+1, y)->getGone())
					drawLine(getParticle(x,y), getParticle(x+1, y));
			//	drawLine(getParticle(x,y), getParticle(x+1, y+1));
			}
	/*		if(x > 0 && y < num_particles_height-1){
				drawLine(getParticle(x,y), getParticle(x-1, y+1));
			}*/
		}
	}
	glEnd();
}

//draw shaded version
void Cloth::drawShaded() {
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++) {
		(*particle).resetNormal();
	}

	for(int x = 0; x<num_particles_width-1; x++) {
		for(int y=0; y<num_particles_height-1; y++) {
			Vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);

			normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
			getParticle(x+1,y+1)->addToNormal(normal);
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);

		/*	normal = calcTriangleNormal(getParticle(x,y+1),getParticle(x,y),getParticle(x+1,y+1));
			getParticle(x,y+1)->addToNormal(normal);
			getParticle(x,y)->addToNormal(normal);
			getParticle(x+1,y+1)->addToNormal(normal);

			normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x+1,y+1));
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y)->addToNormal(normal);
			getParticle(x+1,y+1)->addToNormal(normal);*/
		}
	}

	glBegin(GL_TRIANGLES);
	for(int x = 0; x<num_particles_width-1; x++) {
		for(int y=0; y<num_particles_height-1; y++) {
			Vec3 color(1,1,1);
			drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),color);
			drawTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),color);
	/*		if(x > 0){
				drawTriangle(getParticle(x,y+1),getParticle(x,y),getParticle(x+1,y+1),color);
				drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x+1,y+1),color);
			}*/
		}
	}
	glEnd();
}

// timestep method
void Cloth::timeStep() {
	std::vector<Constraints>::iterator constraint;
	// go through constraints
	for(int i=0; i<CONSTRAINT_ITERATIONS; i++) {
		for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ ) {
			(*constraint).satisfyConstraint(); 
		}
	}

	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++) {
		(*particle).timeStep(); 
	}
}

void Cloth::addForce(const Vec3 direction){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++){
		(*particle).addForce(direction);
	}
}

void Cloth::ballCollision(const Vec3 center, const float radius){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++){
		if(!(*particle).getGone()){
			//std::cout<<"YOOO: " << !(*particle).getGone();
			Vec3 v = (*particle).getPos()-center;
			float l = v.length();
			if(v.length() < radius){
				(*particle).offsetPos(v.normalized()*(radius-1));
					//std::cout << "distance:  " << (*particle).getPos().distance((*particle).getOrigPos());
				if((*particle).getPos().distance((*particle).getOrigPos()) > THETA_DIST){
					(*particle).setGone();
					(*particle).setDisconnectedBottom();
					(*particle).setDisconnectedTop();
					(*particle).setDisconnectedLeft();
					(*particle).setDisconnectedRight();
				}
			}
		}
	}
}
