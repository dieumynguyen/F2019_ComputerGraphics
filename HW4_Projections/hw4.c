#include <stdio.h>

// Custom functions
#include "shapes.h"

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ====================== GLOBALS ====================== */
int th = 0;           //  Azimuth of view angle
int ph = 10;          //  Elevation of view angle
double zh = 0;        //  Rotation
int axes = 1;         //  Display axes

int firstPerson = 1;  // First person mode is perspective
int mode = 1;         // Projection mode: 0 for orthographic, 1 for perspective
int fov = 55;         // Field of view for perspective
double asp = 1;       // Aspect ratio
double dim = 4.0;     // Size of world

// For FP Perspective
float xPosition = 0, yPosition = 0, zPosition = 5;  // Position of camera
float xRotation = 0, yRotation = 0, zRotation = 0;  // Rotation axes
float angle = 0.0;									// Angle of rotation
float cRadius = 2.0;                   				// Our distance from the camera
float xLast, yLast; 								// Previous x,y

/* ====================== PRINT ====================== */
// Convenience routine to output raster text
// Use VARARGS to make this more flexible
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...) {
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*ch++);
}

/* ====================== DISPLAY ====================== */
// OpenGL (GLUT) calls this routine to display the scene
void display() {
	const double len=1.5;  //  Length of axes
	// Erase the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable Z-buffering in OpenGL
	glEnable(GL_DEPTH_TEST);
	//  Undo previous transformations
	glLoadIdentity();

	// Perspective - set eye position
	if (mode && !firstPerson) {
	  double Ex = -2*dim*Sin(th)*Cos(ph);
	  double Ey = +2*dim        *Sin(ph);
	  double Ez = +2*dim*Cos(th)*Cos(ph);
	  gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
	}
	// First person in perspective
	else if (mode && firstPerson) {
		glTranslatef(0.0, 0.0, -cRadius);
		glRotatef(xRotation, 1.0, 0.0, 0.0);
		// Our character to follow
		glColor3f(0.502, 0.502, 0.502);
		cube(0,0,0, 0.2,0.2,0.2, 0,0,0);
		// Rotate camera on y-axis, up/down
		glRotatef(yRotation, 0.0, 1.0, 0.0);
		// Translate screen to the position of camera
		glTranslated(-xPosition, 0.0f, -zPosition);
	}
	//  Orthogonal - set world orientation
	else {
	  glRotatef(ph,1,0,0);
	  glRotatef(th,0,1,0);
	}
	// Set view angle
	// glRotatef(ph,1,0,0);
	// glRotatef(th,0,1,0);

   /* ========= CALL SHAPES HERE ========= */
   ground(0, 50);

   // Bee: x, y, z, azimuth, elevation, theta, scale, head_z, x_scalar, y_scalar, z_scalar
   // Queen bee
   bee(0,0.1,0, 0,0,0, 0.7, 0.35,0.3,0.3,1.5);

   // Worker bees
   bee(1,0.5,0, 15,0,0, 0.7, 0.3,0.3,0.3,1);
   bee(0.6,0.9,0, 30,0,0, 0.7, 0.3,0.3,0.3,1);
   bee(-0.6,0.9,0, 0,60,0, 0.7, 0.3,0.3,0.3,1);
   bee(-1.0,0.7,0.4, 0,30,0, 0.7, 0.3,0.3,0.3,1);
   bee(-1.0,1.2,0.4, 0,0,30, 0.7, 0.3,0.3,0.3,1);

   // Drone bees
   bee(2,0.5,0, 0,0,0, 0.7, 0.35,0.5,0.5,1);
   bee(-2,0.5,0, 0,30,0, 0.7, 0.35,0.5,0.5,1);
   bee(-2,0.1,0.5, 0,0,40, 0.7, 0.35,0.5,0.5,1);
   bee(-1,0.1,0.5, 0,50,0, 0.7, 0.35,0.5,0.5,1);
   /* ========= END SHAPE CALLS ========= */

   //  Draw axes
   glColor3f(1,1,1);
   if (axes) {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s First-Person=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal",firstPerson?"On":"Off");
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
   angle++;
}

/* ====================== PROJECTIONS ====================== */
static void Project() {
	// Tell opengl we want to manipulate projection matrix
	glMatrixMode(GL_PROJECTION);
	// Undo previous transformations
	glLoadIdentity();
	// Projection transformation
	// If first person, always in perspective
	if (mode) {
		gluPerspective(fov, asp, dim/4, 4*dim);
	}
	else {
		glOrtho(-asp*dim, +asp*dim, -dim,+dim, -dim,+dim);
	}
	// Switch to manipulating the model matrix
	glMatrixMode(GL_MODELVIEW);
	// Undo previous transformation
	glLoadIdentity();
}

/* ====================== KEYS & MOUSE ====================== */
// GLUT calls this routine when an arrow key is pressed
void special(int key,int x,int y) {
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// GLUT calls this routine when a key is pressed
void key(unsigned char key, int x, int y) {
	//  Exit on ESC
	if (key == 27) {
		exit(0);
	}
	//  Reset view angle
	else if (key == '0') {
	  th = ph = 0;
  	}
	//  Toggle axes
	else if (key == 'a' || key == 'A') {
		axes = 1-axes;
	}
	//  Switch display mode
	else if (key == 'm' || key == 'M') {
		mode = 1-mode;
	}
	// Turn on/off firstPerson mode
	else if (key == 'p' || key == 'P') {
		firstPerson = 1-firstPerson;
	}
	//  Change field of view angle
	else if (key == '-' && key > 1) {
		fov++;
    }
	// Go up
	if (key == 'q') {
		xRotation += 1;
		if (xRotation > 360) {
			xRotation -= 360;
	  }
	}
	// Go down
	if (key == 'z') {
		xRotation -= 1;
		if (xRotation < -360) {
			xRotation += 360;
	  }
	}
	// Go forward
	if (key == 'w') {
		float xRotRad, yRotRad;
		yRotRad = (yRotation / 180 * 3.141592654f);
		xRotRad = (xRotation / 180 * 3.141592654f);
		xPosition += (float)sin(yRotRad);
		zPosition -= (float)cos(yRotRad);
		yPosition -= (float)sin(xRotRad);
	}
	// Go backward
	if (key == 'x') {
		float xRotRad, yRotRad;
		yRotRad = (yRotation / 180 * 3.141592654f);
		xRotRad = (xRotation / 180 * 3.141592654f);
		xPosition -= (float)sin(yRotRad);
		zPosition += (float)cos(yRotRad);
		yPosition += (float)sin(xRotRad);
	}
	// Go right
	if (key == 'd') {
		float yRotRad;
		yRotRad = (yRotation / 180 * 3.141592654f);
		xPosition += (float)cos(yRotRad) * 0.1;   // 0.2
		zPosition += (float)sin(yRotRad) * 0.1;
	}
	// Go left
	if (key == 'a') {
		float yRotRad;
		yRotRad = (yRotation / 180 * 3.141592654f);
		xPosition -= (float)cos(yRotRad) * 0.1;
		zPosition -= (float)sin(yRotRad) * 0.1;
	}
	if (key == 27) {
		exit(0);
	}
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void mouseMovement(int x, int y) {
	// Check diff bw current and last x position
	int xDiff = x - xLast;
	// Check diff bw current and last y position
	int yDiff = y - yLast;
	// Set last x, y position to current x, y position
	xLast = x;
	yLast = y;
	// Set x rotation with addition of difference in y position
	xRotation += (float) yDiff * 0.1;  // Take off multiplying
	// Set the x rotation
	yRotation += (float) xDiff * 0.1;
}

/* ====================== HELPER FUNCTIONS ====================== */
// GLUT calls this routine when the window is resized
void reshape(int width,int height) {
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

// GLUT calls this toutine when there is nothing else to do
void idle() {
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t, 360);
   glutPostRedisplay();
}

/* ====================== MAIN ====================== */
// Start up GLUT and tell it what to do
int main(int argc,char* argv[]) {
	//  Initialize GLUT and process user parameters
	glutInit(&argc,argv);
	//  Request double buffered, true color window with Z buffering at 600x600
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	//  Create the window
	glutCreateWindow("Dieu My Nguyen - Projections");
	//  Tell GLUT to call "idle" when there is nothing else to do
	glutIdleFunc(idle);
	//  Tell GLUT to call "display" when the scene should be drawn
	glutDisplayFunc(display);
	//  Tell GLUT to call "reshape" when the window is resized
	glutReshapeFunc(reshape);
	// Check for mouse movement
	glutPassiveMotionFunc(mouseMovement);
	//  Tell GLUT to call "special" when an arrow key is pressed
	glutSpecialFunc(special);
	//  Tell GLUT to call "key" when a key is pressed
	glutKeyboardFunc(key);
	//  Pass control to GLUT so it can interact with the user
	glutMainLoop();
	return 0;
}
