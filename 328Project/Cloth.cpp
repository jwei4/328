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

void Cloth::drawLine(int index1, int index2){
	glNormal3fv((GLfloat *) &(particles.at(index1).getNormal().normalized()));
	glVertex3fv((GLfloat *) &(particles.at(index1).getPos() ));

	glNormal3fv((GLfloat *) &(particles.at(index2)).getNormal().normalized());
	glVertex3fv((GLfloat *) &(particles.at(index2).getPos() ));
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
	//particles.resize(num_particles_width*num_particles_height);
	particles.clear();

	// creating particles from (0,0,0) to (width,-height,0)
	for(int x=0; x<num_particles_width; x++) {
		for(int y=0; y<num_particles_height; y++) {
			Vec3 pos = Vec3(width * (x/(float)num_particles_width),-height * (y/(float)num_particles_height),0);
			Particle *TBA = new Particle(pos);
			TBA->setIndex(particles.size());
			particles.push_back(*TBA); // insert into particle vector
		//	Particle* p = new Particle(particles.back());
			//std::cout<<"index: " << particles.size()<< " x: " << (*p).getPos().f[0] << " y: " <<(*p).getPos().f[1] << " z: " <<(*p).getPos().f[2] << "\n";

			//set particle's neighbors 
			if((getParticle(x,y))->getPos().f[0] == 0){
				getParticle(x,y)->setLeft(-1);
			}else if ((getParticle(x,y))->getPos().f[0] == (num_particles_width-1)){
				getParticle(x,y)->setRight(-1);
				getParticle(x,y)->setLeft(x*num_particles_height+y-num_particles_height);
			}else{
				getParticle(x,y)->setLeft(x*num_particles_height+y-num_particles_height);
				(particles.at(x*num_particles_height+y-num_particles_height)).setRight(x*num_particles_height+y);
			}

			if(abs(getParticle(x,y)->getPos().f[1]) == 0){
				getParticle(x,y)->setTop(-1);
			}else if(abs(getParticle(x,y)->getPos().f[1] == (height-1))){
				getParticle(x,y)->setBottom(-1);
				getParticle(x,y)->setTop(y-1);
			}
			else{
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
			if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getRightP(x,y));
			if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getBotP(x,y));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
			if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getRightP(x,y),getBotP(x,y));
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
	for(int i=0;i<3; i++)
	//for(int i = 0; i < num_particles_height; i++)
	{
		//top row
		getParticle(0+i ,0)->makeUnmovable(); 
		getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		getParticle(0+i, num_particles_height-1)->makeUnmovable();
		getParticle(num_particles_width-1-i, num_particles_height-1)->makeUnmovable();

		//getParticle(0, 0+i)->makeUnmovable();
		//getParticle(num_particles_width-1, 0+i)->makeUnmovable();
	}
}

//draw Wire version
void Cloth::drawWire(){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++) {
		(*particle).resetNormal();
	}
	//use vectors with particles that keep track of neighbors, (look into knn algorithm)
	glBegin(GL_LINES);
	//std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle < particles.end(); particle++){
		if(!(*particle).getGone()){
			if((*particle).getTop() != -1 && (*particle).getTop() != NULL)
				drawLine(particle-particles.begin(), (*particle).getTop());
			
			if((*particle).getBottom() != -1 && (*particle).getBottom() != NULL)
				drawLine(particle-particles.begin(), (*particle).getBottom());
			
			if((*particle).getLeft() != -1 && (*particle).getLeft() != NULL)
				drawLine(particle-particles.begin(), (*particle).getLeft());
			
			if((*particle).getRight() != -1 && (*particle).getRight() != NULL)
				drawLine(particle-particles.begin(), (*particle).getRight());
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

void Cloth::transitionModel(Particle &p){
	int top = p.getTop();
	int bottom = p.getBottom();
/*	if(p.getTop() == -1 || p.getRight() == -1 || p.getLeft() == -1 || p.getBottom() == -1){
		if(p.getWarp() == 0 && p.getWeft() == 0){
			Vec3 warp = p.getPos() + (Vec3(1,1,1)*(THICKNESS/2));
			Vec3 weft = p.getPos() - (Vec3(1,1,1)*(THICKNESS/2));
			//std::cout << "warp: " << warp.f[0] << warp.f[1] << warp.f[2] << " weft: "<< weft.f[0] << weft.f[1] << weft.f[2] << "\n";

			p.setGone();
			particles.push_back(Particle(warp));
			if(p.getTop() == -1){(particles.at(particles.size()-1)).setTop(-1);}
			else{
				(particles.at(p.getTop())).setBottom(particles.size()-1);
				(particles.at(particles.size()-1)).setTop(p.getTop());

				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getTop()));
			}
			if(p.getBottom() == -1){(particles.at(particles.size()-1)).setBottom(-1);}
			else{
				(particles.at(p.getBottom())).setTop(particles.size()-1);
				(particles.at((particles.size()-1))).setBottom(p.getBottom());
				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getBottom()));
			}
			(particles.at(particles.size()-1)).setLeft(-1);
			(particles.at(particles.size()-1)).setRight(-1);
			(particles.at(particles.size()-1)).setWarpHigh();
			//std::cout << "warp: "<< (particles.at(particles.size()-1)).getWarp() << "\n";



			particles.push_back(Particle(weft));
			if(p.getRight() == -1){(particles.at(particles.size()-1)).setRight(-1);}
			else{
				(particles.at(p.getRight())).setLeft(particles.size()-1);
				(particles.at((particles.size()-1))).setRight(p.getRight());
				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getRight()));
			}
			if(p.getLeft() == -1){(particles.at(particles.size()-1)).setLeft(-1);}
			else{
				(particles.at(p.getLeft())).setRight(particles.size()-1);
				(particles.at((particles.size()-1))).setLeft(p.getLeft());
				makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getLeft()));
			}
			(particles.at(particles.size()-1)).setTop(-1);
			(particles.at(particles.size()-1)).setBottom(-1);
			(particles.at(particles.size()-1)).setWeftHigh();

/*
			if(neighbor == 0){
				neighbor = 1;
				if(!(p.getTop() == -1))
					transitionModel(&(particles.at(p.getTop())));
				if(!(p.getBottom() == -1))
					transitionModel(&(particles.at(p.getBottom())));
				if(!(p.getLeft() == -1))
					transitionModel(&(particles.at(p.getLeft())));
				if(!(p.getRight() == -1))
					transitionModel(&(particles.at(p.getRight())));
			}
		}
	}
	

	else*/ if(p.getWarp() == 0 && p.getWeft() == 0 && p.getTop() > -1 && p.getRight() > -1 && p.getLeft() > -1 && p.getBottom() > -1 && p.getMovable() && !p.getGone()){
		Vec3 *clothNormal = new Vec3(calcClothNormal(&p));
		Vec3 *warp = new Vec3(p.getPos() + (*clothNormal*(THICKNESS/2)));
		Vec3 *weft = new Vec3(p.getPos() - (*clothNormal*(THICKNESS/2)));
		std::cout<<" WARP: "<< warp->f[0] << " " << warp->f[1] << " "<< warp->f[2] << "\n";
		std::cout<<" WEFT: "<< weft->f[0] << " " << weft->f[1] << " "<< weft->f[2] << "\n";
		p.setGone();
		Particle *TBA = new Particle(*warp);
		TBA->setTop(p.getTop());
		TBA->setBottom(p.getBottom());
		TBA->setLeft(-1);
		TBA->setRight(-1);
		TBA->setWarpHigh();
		TBA->setIndex(particles.size());
		particles.push_back(*TBA);
		int size = (particles.size())-1;

		std::cout<<"index: " << particles.size()<< " x: " << particles.back().getPos().f[0] << " y: " << particles.back().getPos().f[1]  << " z: " << particles.back().getPos().f[2] << "\n";
		std::cout<< " INDEX: "<<p.getIndex() << "\n";
		//std::cout << "TOP: "<<p.getTop() << "  " << p.getWeft() << " " << p.getWarp();
		particles.at(p.getTop()).setBottom(size);
		particles.at(p.getBottom()).setTop(size);

		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getTop()));
		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getBottom()));

		Particle *TBAA = new Particle(*weft);
		TBAA->setLeft(p.getLeft());
		TBAA->setRight(p.getRight());
		TBAA->setTop(-1);
		TBAA->setBottom(-1);
		TBAA->setWeftHigh();
		TBAA->setIndex(particles.size());
		particles.push_back(*TBAA);
		size = (particles.size())-1;
		
		std::cout<<"index: " << particles.size()<< " x: " << particles.back().getPos().f[0] << " y: " << particles.back().getPos().f[1] << " z: " << particles.back().getPos().f[2] << "\n";
		
		particles.at(p.getLeft()).setRight(size);
		particles.at(p.getRight()).setLeft(size);
		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getLeft()));
		makeConstraint(&(particles.at(particles.size()-1)), &particles.at((particles.at(particles.size()-1)).getRight()));

	/*	if(neighbor == 0){
			neighbor = 1;
			if(p.getTop() != -1 && particles.at(p.getTop()).getWarp() == 0 && particles.at(p.getTop()).getWeft() == 0)
			transitionModel((particles.at(p.getTop())));
			if(p.getBottom() != -1 && particles.at(p.getBottom()).getWarp() == 0 && particles.at(p.getBottom()).getWeft() == 0)
			transitionModel((particles.at(p.getBottom())));
			if(p.getLeft() != -1 && particles.at(p.getLeft()).getWarp() == 0 && particles.at(p.getLeft()).getWeft() == 0)
			transitionModel((particles.at(p.getLeft())));
			if(p.getRight() != -1 && particles.at(p.getRight()).getWarp() == 0 && particles.at(p.getRight()).getWeft() == 0)
			transitionModel((particles.at(p.getRight())));
		}*/
	}
}

void Cloth::checkTearDistance(Particle* p){
	neighbor = 0;
	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
			//transitionModel(particles.at((*p).getTop()));
		}
	}

	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
		//	transitionModel(particles.at((*p).getBottom()));
		}
	}

	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
		//	transitionModel(particles.at((*p).getRight()));
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
			//transitionModel(particles.at((*p).getLeft()));
		}
	}
}


/*
void Cloth::checkTearDistance(Particle* p){
	neighbor = 0;
	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
			if((*p).getBottom() != -1)
			transitionModel((particles.at((*p).getBottom())));
			
			if((*p).getLeft() != -1)
			transitionModel((particles.at((*p).getLeft())));
			
			if((*p).getRight() != -1)
			transitionModel((particles.at((*p).getRight())));

			transitionModel(particles.at((*p).getTop()));

			if(particles.at((*p).getTop()).getLeft() != -1 && particles.at((particles.at((*p).getTop())).getLeft()).getWarp() == 0 && particles.at((particles.at((*p).getTop())).getLeft()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getTop())).getLeft()));

			if(particles.at((*p).getTop()).getRight() != -1 && particles.at((particles.at((*p).getTop())).getRight()).getWarp() == 0 && particles.at((particles.at((*p).getTop())).getRight()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getTop())).getRight()));

			if(particles.at((*p).getTop()).getTop() != -1 && particles.at((particles.at((*p).getTop())).getTop()).getWarp() == 0 && particles.at((particles.at((*p).getTop())).getTop()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getTop())).getTop()));

		}
	}

	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
			
			if((*p).getLeft() != -1)
			transitionModel((particles.at((*p).getLeft())));
			
			if((*p).getRight() != -1)
			transitionModel((particles.at((*p).getRight())));
			
			if((*p).getTop() != -1)
			transitionModel((particles.at((*p).getTop())));

			transitionModel(particles.at((*p).getBottom()));

			if(particles.at((*p).getBottom()).getLeft() != -1 && particles.at((particles.at((*p).getBottom())).getLeft()).getWarp() == 0 && particles.at((particles.at((*p).getBottom())).getLeft()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getBottom())).getLeft()));

			if(particles.at((*p).getBottom()).getRight() != -1 && particles.at((particles.at((*p).getBottom())).getRight()).getWarp() == 0 && particles.at((particles.at((*p).getBottom())).getRight()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getBottom())).getRight()));

			if(particles.at((*p).getBottom()).getBottom() != -1 && particles.at((particles.at((*p).getBottom())).getBottom()).getWarp() == 0 && particles.at((particles.at((*p).getBottom())).getBottom()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getBottom())).getBottom()));
		}
	}

	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > TEAR_THRESH){
			transitionModel((*p));
			
			if((*p).getLeft() != -1)
			transitionModel((particles.at((*p).getLeft())));
			
			if((*p).getBottom() != -1)
			transitionModel((particles.at((*p).getBottom())));
			
			if((*p).getTop() != -1)
			transitionModel((particles.at((*p).getTop())));

			transitionModel(particles.at((*p).getRight()));

			if(particles.at((*p).getRight()).getTop() != -1 && particles.at((particles.at((*p).getRight())).getTop()).getWarp() == 0 && particles.at((particles.at((*p).getRight())).getTop()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getRight())).getTop()));

			if(particles.at((*p).getRight()).getRight() != -1 && particles.at((particles.at((*p).getRight())).getRight()).getWarp() == 0 && particles.at((particles.at((*p).getRight())).getRight()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getRight())).getRight()));

			if(particles.at((*p).getRight()).getBottom() != -1 && particles.at((particles.at((*p).getRight())).getBottom()).getWarp() == 0 && particles.at((particles.at((*p).getRight())).getBottom()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getRight())).getBottom()));
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > TEAR_THRESH){
			transitionModel((*p));

			if((*p).getRight() != -1)
			transitionModel((particles.at((*p).getRight())));
			
			if((*p).getBottom() != -1)
			transitionModel((particles.at((*p).getBottom())));
			
			if((*p).getTop() != -1)
			transitionModel((particles.at((*p).getTop())));

			transitionModel(particles.at((*p).getLeft()));

			if(particles.at((*p).getLeft()).getTop() != -1 && particles.at((particles.at((*p).getLeft())).getTop()).getWarp() == 0 && particles.at((particles.at((*p).getLeft())).getTop()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getLeft())).getTop()));

			if(particles.at((*p).getLeft()).getLeft() != -1&& particles.at((particles.at((*p).getLeft())).getLeft()).getWarp() == 0 && particles.at((particles.at((*p).getLeft())).getLeft()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getLeft())).getLeft()));

			if(particles.at((*p).getLeft()).getBottom() != -1 && particles.at((particles.at((*p).getLeft())).getBottom()).getWarp() == 0 && particles.at((particles.at((*p).getLeft())).getBottom()).getWeft() == 0)
			transitionModel(particles.at(particles.at(((*p).getLeft())).getBottom()));
		}
	}
}*/

void Cloth::checkRipDistance(Particle* p){

	if((*p).getWarp() == 0 && (*p).getWeft() == 0){

	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > THETA_DIST){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getTop()).setBottom(-1);
			(*p).setTop(-1);
		}
	}
	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > THETA_DIST){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getBottom()).setTop(-1);
			(*p).setBottom(-1);
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > THETA_DIST){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getLeft()).setRight(-1);
			(*p).setLeft(-1);
		}
	}
	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > THETA_DIST){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getRight()).setLeft(-1);
			(*p).setRight(-1);
		}
			}
	}
}

void Cloth::checkRipDistance2(Particle* p){

	if((*p).getWarp() == 0 && (*p).getWeft() == 0){

	if((*p).getTop() != -1){
		if((*p).getPos().distance((particles.at((*p).getTop()).getPos())) > 1.5){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getTop()).setBottom(-1);
			(*p).setTop(-1);
		}
	}
	if((*p).getBottom() != -1){
		if((*p).getPos().distance((particles.at((*p).getBottom()).getPos())) > 1.5){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getBottom()).setTop(-1);
			(*p).setBottom(-1);
		}
	}
	if((*p).getLeft() != -1){
		if((*p).getPos().distance((particles.at((*p).getLeft()).getPos())) > 1.5){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getLeft()).setRight(-1);
			(*p).setLeft(-1);
		}
	}
	if((*p).getRight() != -1){
		if((*p).getPos().distance((particles.at((*p).getRight()).getPos())) > 1.5){
			//(*p).setGone();
			//(*p).setNoConstraints();
			particles.at((*p).getRight()).setLeft(-1);
			(*p).setRight(-1);
		}
			}
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
			std::cout<< "INDEX: " << distance(particles.begin(), particle) << " SIZEE: " << particles.size()<< "\n"; 
			std::cout<<"TOP: "<<(*particle).getTop()<< " BOT: " << (*particle).getBottom() << " LEFT: " << (*particle).getLeft() << " RIGHT: " << (*particle).getRight() << "\n";
				checkTearDistance(&(*particle));
				checkRipDistance(&(*particle));
			}
		}
	}
}

void Cloth::ballCollision2(const Vec3 center, const float radius){
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++){
		if(!(*particle).getGone()){
			Vec3 v = (*particle).getPos()-center;
			float l = v.length();
			if(v.length() < radius){
				(*particle).offsetPos(v.normalized()*(radius-1));
			std::cout<< "INDEX: " << distance(particles.begin(), particle) << " SIZEE: " << particles.size()<< "\n"; 
			std::cout<<"TOP: "<<(*particle).getTop()<< " BOT: " << (*particle).getBottom() << " LEFT: " << (*particle).getLeft() << " RIGHT: " << (*particle).getRight() << "\n";
				checkTearDistance(&(*particle));
				checkRipDistance2(&(*particle));
			}
		}
	}
}