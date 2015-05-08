#ifndef Project_H
#define Project_H

#define DAMPING 0.01				// Damping for particle
#define TIME_STEPSIZE2 0.5*0.5		// timestep for particle
#define CONSTRAINT_ITERATIONS 15    // iterations of constraint satisfaction each frame
#define THICKNESS .5				// how far each particle is seperated
#define THETA_DIST 1.1				// distance at which particles disconnect
#define TEAR_THRESH .5				// distance at which particles start to tear

class Project {
	void drawThings();
	void Mouse(int button, int state, int x, int y);
	void MouseMotion(int x, int y);
	void keyboard( unsigned char key, int x, int y );
	void winReshapeFcn (int w, int h);
	void display(void);
	int main (int argc, char** argv);
};

#endif
