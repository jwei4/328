#include "Project.h"
#include "Vec3.h"
#include "Particle.h"
#include "Constraints.h"
#include "Cloth.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GL.h>
using namespace std;

static float mouse_x(0.0), mouse_y(0.0);
static float rotation_x(0.0),rotation_y(0.0);
static int win_width(1280), win_height(700);
static float thickness = 0.01, arealDensity  = 0.25, k_struct = 6000, k_bend = 5.0, k_shear = 2.0, theta_dist = 0.02;
float yarnPerUnitLength = 10;		//10-15 cm
float tearingThreshold = 5;			//5-10% 
float modelTransThreshold = 4.5;	//4.5-9%
float N_proj = 30;					//30-50

int dragging = 1;

Cloth cloth1(14,10,55,45); // one Cloth object of the Cloth class
Vec3 ball_pos(7,-5,0);
float ball_radius = 2;

//Draw Stuff
void drawThings() {
	//glutWireCube(2.0);
	cloth1.drawShaded();
	//cloth1.drawWire(); 
}

void Mouse(int button, int state, int x, int y) {
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		dragging = 1;
        mouse_x = (float) x;
        mouse_y = (float) y;
    }
	else{
		dragging = 0;
	}
}

// Call-back function glutMouseFunc();
void MouseMotion(int x, int y) {
	if(dragging){
    rotation_x += (float) x - mouse_x;
    rotation_y += (float) y - mouse_y;

    mouse_x = (float) x;
    mouse_y = (float) y;

    glutPostRedisplay(); // for redraw the current mouse position;
	}
};

void keyboard( unsigned char key, int x, int y ) {
	switch ( key ) {
	case 27:    
		exit ( 0 );
		break;  
	default: 
		break;
	}
	//wireframe
	if(key == 'w'){
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glutPostRedisplay();	
		}
	//shaded
	if(key == 's'){
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glutPostRedisplay();	
		}
};

//Window reshape function
void winReshapeFcn (int w, int h) {
/*	glViewport (0, 0, newWidth, newHeight);
	glMatrixMode (GL_PROJECTION);
	glFrustum (-1.0, 1.0, -1.0, 1.0, 2.0, 20.0);
	glMatrixMode (GL_MODELVIEW);
	glClear (GL_COLOR_BUFFER_BIT);*/

	win_width = w;
    win_height = h;

    //update the viewport;
    glViewport(0,0,(GLsizei)w,(GLsizei)h);

    //update the projection matrix;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(float)win_width/win_height,0.01,50);
	gluLookAt(5.0,5.0,5.0, //eye
              0.0,0.0,0.0, //center
              0.0,1.0,0.0); //upY

    glMatrixMode(GL_MODELVIEW);
}

float ball_time = 0;

void display(void) {

	ball_time++;
	ball_pos.f[2] = cos(ball_time/50.0)*7;

	cloth1.addForce(Vec3(0,-0.2,0)*TIME_STEPSIZE2);
	//cloth1.windForce(Vec3(0.5, 0, 0.2)*TIME_STEPSIZE2);
	cloth1.timeStep();
	cloth1.ballCollision(ball_pos, ball_radius);

	//clean the scene
    glClearColor(0.3,0.3,0.3,1.0); //set background
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //set matrix mode to GL_MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    
    glColor3f(1.0f,0.4f,0.0f);

    glPushMatrix();
    glLoadIdentity();
    glRotatef(rotation_x,0.0,1.0,0.0);
    glRotatef(rotation_y,1.0,0.0,0.0);
	cloth1.timeStep(); 
	glTranslatef(-10.5,0,-1.0f);
	glRotatef(30,-5,5, 1);
	drawThings();
   
	glTranslatef(ball_pos.f[0], ball_pos.f[1], ball_pos.f[2]);
	glColor3f(0.4f, 0.8f, 0.5f);
	glutSolidSphere(ball_radius-0.1,50,50);

	glPopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

int main (int argc, char** argv) {
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (50, 50);
	glutInitWindowSize (win_width,win_height);
	glutCreateWindow ("Jackie Wei CS 328 Project");

	glutDisplayFunc (display);
	glutReshapeFunc (winReshapeFcn);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
}
