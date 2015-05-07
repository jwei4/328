#include "Vec3.h"
#include "Project.h"
#include "Cloth.h"
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <iostream>

Particle* Cloth::getParticle(int x, int y) {return &particles[x*num_particles_height + y];}
Particle* Cloth::getTopP(int x, int y) {return &particles[x*num_particles_height+y-1];}
Particle* Cloth::getBotP(int x, int y) {return &particles[x*num_particles_height+y+1];}
Particle* Cloth::getLeftP(int x, int y) {return &particles[x*num_particles_height+y-num_particles_height];}
Particle* Cloth::getRightP(int x, int y) {return &particles[x*num_particles_height + y+num_particles_height];}
void Cloth::makeConstraint(Particle *p1, Particle *p2) {constraints.push_back(Constraints(p1,p2));}

std::vector<Particle>::iterator particle;

int neighbor = 0;

int Cloth::getVecIndex(int x, int y, int z){
	int i = 0;
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++){
		if((*particle).getPos().f[0] == x && (*particle).getPos().f[1] == y && (*particle).getPos().f[2] == z)
			return i;
		i++;
	}
	return -1;
}

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

Vec3 Cloth::calcClothNormal(Particle *p){

	if((*p).getBottom() == -1 || (*p).getTop() == -1 || (*p).getLeft() == -1 || (*p).getRight() == -1){}
	else if(!(*p).getGone()){
		Vec3 a = ((particles.at((*p).getTop()).getPos())-(particles.at((*p).getLeft()).getPos())).cross((*p).getPos()-(particles.at((*p).getLeft()).getPos()));
		Vec3 b = ((particles.at((*p).getRight()).getPos())-(particles.at((*p).getTop()).getPos())).cross((*p).getPos()-(particles.at((*p).getTop()).getPos()));
		Vec3 c = ((particles.at((*p).getBottom()).getPos())-(particles.at((*p).getRight()).getPos())).cross((*p).getPos()-(particles.at((*p).getRight()).getPos()));
		Vec3 d = ((particles.at((*p).getLeft()).getPos())-(particles.at((*p).getBottom()).getPos())).cross((*p).getPos()-(particles.at((*p).getBottom()).getPos()));
		return ((a+b+c+d)/4);
	}
}

Cloth::Cloth(float width, float height) : num_particles_width(width/THICKNESS), num_particles_height(height/THICKNESS){
	std::cout << "width: " << num_particles_width << " height: " << num_particles_height;

	// creating particles from (0,0,0) to (width,-height,0)
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			Vec3 pos = Vec3(width * (x/(float)num_particles_width),-height * (y/(float)num_particles_height),0);
			particles.push_back(Particle(pos)); // insert into particle vector

			//set particle's neighbors 
			if(getParticle(x,y)->getPos().f[0] == 0){
				getParticle(x,y)->setLeft(-1);
			}else{
				getParticle(x,y)->setLeft(x*num_particles_height+y-num_particles_height);
				particles.at(x*num_particles_height+y-num_particles_height).setRight(x*num_particles_height+y);
			}

			if(abs(getParticle(x,y)->getPos().f[1]) == 0){
				getParticle(x,y)->setTop(-1);
			}else{
				getParticle(x,y)->setTop(x*num_particles_height+y-1);
				particles.at(x*num_particles_height+y-1).setBottom(x*num_particles_height+y);
				/*
				getParticle(x,y)->setTop(y-1);
				particles.at(y-1).setBottom(y);*/
			}
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
	for(int i = 0; i < num_particles_height; i++)
	{
		//top row
		/*getParticle(0+i ,0)->makeUnmovable(); 
		getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		getParticle(0+i, num_particles_height-1)->makeUnmovable();
		getParticle(num_particles_width-1-i, num_particles_height-1)->makeUnmovable();*/

		getParticle(0, 0+i)->makeUnmovable();
		getParticle(num_particles_width-1, 0+i)->makeUnmovable();
	}
}

//draw Wire version
void Cloth::drawWire(){

	for(particle = particles.begin(); particle != particles.end(); particle++) {
		(*particle).resetNormal();
	}
	//use vectors with particles that keep track of neighbors, (look into knn algorithm)
	glBegin(GL_LINES);
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle < particles.end(); particle++){
		if(!(*particle).getGone()){
			/*	if((*particle).getWeft() > 0 || (*particle).getWarp() > 0)
			std::cout << (*particle).getWeft() << "\n";*/
			if(!((*particle).getTop() == -1))
				drawLine(&(*particle), &(particles.at((*particle).getTop())));
			if(!((*particle).getBottom() == -1))
				drawLine(&(*particle), &(particles.at((*particle).getBottom())));
			if(!((*particle).getLeft() == -1))
				drawLine(&(*particle), &(particles.at((*particle).getLeft())));
			if(!((*particle).getRight() == -1))
				drawLine(&(*particle), &(particles.at((*particle).getRight())));
		}
	}
	/*for(int x = 0; x < num_particles_width; x++) {
	for(int y=0; y < num_particles_height; y++) {
	if(x == num_particles_width-1 && y < num_particles_height-1){
	if(!getParticle(x,y)->getGone() && !((particles.at(getParticle(x,y)->getTop())).getGone()))
	drawLine(getParticle(x,y), &(particles.at(getParticle(x,y)->getTop())));
	}
	if(y == num_particles_height-1 && x < num_particles_width-1){
	if(!getParticle(x,y)->getGone() && !((particles.at(getParticle(x,y)->getRight())).getGone()))
	drawLine(getParticle(x,y), &(particles.at(getParticle(x,y)->getRight())));
	}
	if(x < num_particles_width-1 && y < num_particles_height-1){
	if(!getParticle(x,y)->getGone() && !((particles.at(getParticle(x,y)->getBottom())).getGone()))
	drawLine(getParticle(x,y), &(particles.at(getParticle(x,y)->getBottom())));
	if(!getParticle(x,y)->getGone() && !((particles.at(getParticle(x,y)->getRight())).getGone()))
	drawLine(getParticle(x,y), &(particles.at(getParticle(x,y)->getRight())));
	}
	}
	}*/
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

void Cloth::transitionModel(Particle* p){
	if((*p).getTop() == -1 || (*p).getRight() == -1 || (*p).getLeft() == -1 || (*p).getBottom() == -1){
		if((*p).getWarp() == 0 && (*p).getWeft() == 0){
			Vec3 warp = (*p).getPos() + (Vec3(1,1,1)*(THICKNESS/2));
			Vec3 weft = (*p).getPos() - (Vec3(1,1,1)*(THICKNESS/2));

			(*p).setGone();
			particles.push_back(Particle(warp));
			if((*p).getTop() == -1){(particles.at(particles.size()-1)).setTop(-1);}
			else{
				particles.at((*p).getTop()).setBottom(particles.size()-1);
				(particles.at(particles.size()-1)).setTop((*p).getTop());

				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getTop()));
			}
			if((*p).getBottom() == -1){(particles.at(particles.size()-1)).setBottom(-1);}
			else{
				particles.at((*p).getBottom()).setTop(particles.size()-1);
				(particles.at(particles.size()-1)).setBottom((*p).getBottom());

				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getBottom()));
			}
			(particles.at(particles.size()-1)).setWarpHigh();


			particles.push_back(Particle(weft));
			if((*p).getRight() == -1){(particles.at(particles.size()-1)).setRight(-1);}
			else{
				particles.at((*p).getRight()).setLeft(particles.size()-1);
				(particles.at(particles.size()-1)).setRight((*p).getRight());
				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getRight()));
			}
			if((*p).getLeft() == -1){(particles.at(particles.size()-1)).setLeft(-1);}
			else{
				particles.at((*p).getLeft()).setRight(particles.size()-1);
				(particles.at(particles.size()-1)).setLeft((*p).getLeft());
				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getLeft()));
			}
			(particles.at(particles.size()-1)).setWeftHigh();


			if(neighbor == 0){
				neighbor = 1;
				if(!((*p).getTop() == -1))
					transitionModel(&(particles.at((*p).getTop())));
				if(!((*p).getBottom() == -1))
					transitionModel(&(particles.at((*p).getBottom())));
				if(!((*p).getLeft() == -1))
					transitionModel(&(particles.at((*p).getLeft())));
				if(!((*p).getRight() == -1))
					transitionModel(&(particles.at((*p).getRight())));
			}
		}
	}

	else if((*p).getWarp() == 0 && (*p).getWeft() == 0){
		Vec3 clothNormal = calcClothNormal(&(*p));
		Vec3 warp = (*p).getPos() + (clothNormal*(THICKNESS/2));
		Vec3 weft = (*p).getPos() - (clothNormal*(THICKNESS/2));

		/*	std::cout << "P X: " << (*p).getPos().f[0] << " Y: " << (*p).getPos().f[1] << " Z: " << (*p).getPos().f[2];
		std::cout << "warP X: " << warp.f[0] << " Y: " << warp.f[1] << " Z: " << warp.f[2];
		std::cout << "weft X: " << weft.f[0] << " Y: " << weft.f[1] << " Z: " << weft.f[2] << "\n";*/

		(*p).setGone();
		particles.push_back(Particle(warp));
		particles.at((*p).getTop()).setBottom(particles.size()-1);
		particles.at((*p).getBottom()).setTop(particles.size()-1);
		(particles.at(particles.size()-1)).setTop((*p).getTop());
		(particles.at(particles.size()-1)).setBottom((*p).getBottom());
		(particles.at(particles.size()-1)).setWarpHigh();

		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getTop()));
		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getBottom()));

		particles.push_back(Particle(weft));
		particles.at((*p).getLeft()).setRight(particles.size()-1);
		particles.at((*p).getRight()).setLeft(particles.size()-1);
		(particles.at(particles.size()-1)).setLeft((*p).getLeft());
		(particles.at(particles.size()-1)).setRight((*p).getRight());
		(particles.at(particles.size()-1)).setWeftHigh();

		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getLeft()));
		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getRight()));

		if(neighbor == 0){
			neighbor = 1;
			transitionModel(&(particles.at((*p).getTop())));
			transitionModel(&(particles.at((*p).getBottom())));
			transitionModel(&(particles.at((*p).getLeft())));
			transitionModel(&(particles.at((*p).getRight())));
		}
	}
}

void Cloth::checkTearDistance(Particle* p){
	neighbor = 0;
	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > TEAR_THRESH){
			transitionModel(&(*p));
			transitionModel(&particles.at((*p).getTop()));
		}
	}

	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > TEAR_THRESH){
			transitionModel(&(*p));
			transitionModel(&particles.at((*p).getBottom()));
		}
	}

	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > TEAR_THRESH){
			transitionModel(&(*p));
			transitionModel(&particles.at((*p).getRight()));
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > TEAR_THRESH){
			transitionModel(&(*p));
			transitionModel(&particles.at((*p).getLeft()));
		}
	}
}

void Cloth::checkRipDistance(Particle* p){

	//if((*p).getWarp() == 0 && (*p).getWeft() == 0){

	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > THETA_DIST){
			//(*p).setGone();
			(*p).setNoConstraints();
			particles.at((*p).getTop()).setBottom(-1);
			(*p).setTop(-1);
		}
	}
	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > THETA_DIST){
			//(*p).setGone();
			(*p).setNoConstraints();
			particles.at((*p).getBottom()).setTop(-1);
			(*p).setBottom(-1);
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > THETA_DIST){
			//(*p).setGone();
			(*p).setNoConstraints();
			particles.at((*p).getLeft()).setRight(-1);
			(*p).setLeft(-1);
		}
	}
	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > THETA_DIST){
			//(*p).setGone();
			(*p).setNoConstraints();
			particles.at((*p).getRight()).setLeft(-1);
			(*p).setRight(-1);
		}
		//	}
	}
}

void Cloth::ballCollision(const Vec3 center, const float radius){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++){
		if(!(*particle).getGone()){
			Vec3 v = (*particle).getPos()-center;
			float l = v.length();
			if(v.length() < radius){
				(*particle).offsetPos(v.normalized()*(radius-1));
				checkTearDistance(&(*particle));
				checkRipDistance(&(*particle));
			}
		}
	}
}
