#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th = -40;         //  Azimuth of view angle
int ph = -340;         //  Elevation of view angle
double zh = 0;      //  Rotation of teapot
int axes = 1;       //  Display axes

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

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
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/* ====================== SHAPES ====================== */
// ====================== CUBOID
void cube(double x, double y, double z,
          double dx, double dy, double dz,
          double azimuth, double elevation, double theta) {
  // Cube vertices
  GLfloat vertA[3] = { 0.5, 0.5, 0.5};
  GLfloat vertB[3] = {-0.5, 0.5, 0.5};
  GLfloat vertC[3] = {-0.5,-0.5, 0.5};
  GLfloat vertD[3] = { 0.5,-0.5, 0.5};
  GLfloat vertE[3] = { 0.5, 0.5,-0.5};
  GLfloat vertF[3] = {-0.5, 0.5,-0.5};
  GLfloat vertG[3] = {-0.5,-0.5,-0.5};
  GLfloat vertH[3] = { 0.5,-0.5,-0.5};

  glPushMatrix();

  // Transform cube
  glTranslatef(x,y,z);
  glRotatef(azimuth,1,0,0);
  glRotatef(elevation,0,1,0);
  glRotatef(theta,0,0,1);
  glScalef(dx,dy,dz);

  glColor3f(0.855, 0.647, 0.125);
  glBegin(GL_QUADS);
  // Front
  glVertex3fv(vertA);
  glVertex3fv(vertB);
  glVertex3fv(vertC);
  glVertex3fv(vertD);
  // Back
  glVertex3fv(vertF);
  glVertex3fv(vertE);
  glVertex3fv(vertH);
  glVertex3fv(vertG);
  // Right
  glVertex3fv(vertE);
  glVertex3fv(vertA);
  glVertex3fv(vertD);
  glVertex3fv(vertH);
  // Left
  glVertex3fv(vertB);
  glVertex3fv(vertF);
  glVertex3fv(vertG);
  glVertex3fv(vertC);
  // Top
  glVertex3fv(vertE);
  glVertex3fv(vertF);
  glVertex3fv(vertB);
  glVertex3fv(vertA);
  // Bottom
  glVertex3fv(vertD);
  glVertex3fv(vertC);
  glVertex3fv(vertG);
  glVertex3fv(vertH);
  glEnd();

  glPopMatrix();
}

// ====================== SPHERE
static void Vertex(double th,double ph) {
  // Angle th and ph are in radians
  glVertex3d(Sin(th)*Cos(ph), Sin(ph), Cos(th)*Cos(ph));
}

static void sphere(double x, double y, double z, double r,
                   double x_scalar, double y_scalar, double z_scalar,
                   bool head) {
   const int d=15;
   int th, ph;

   glPushMatrix();

   //  Offset and scale
   glTranslated(x, y, z);
   glScaled(x_scalar*r, y_scalar*r, z_scalar*r);

   // Default brown color
   glColor3f(0.545, 0.271, 0.075);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0, -90);
   for (th=0; th<=360; th+=d) {
      Vertex(th, d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90; ph<=90-2*d; ph+=d) {
     // For alternating color for yellow stripes
     int strip_width = 2;
     int strip_counter = strip_width;
     int color_idx = 0;

      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=d) {
        if (!head) {
          // Switch to yellow
          if (strip_counter == 0) {
            color_idx = 1 - color_idx;
            if (color_idx) {
              glColor3f(0.545, 0.271, 0.075);
            }
            else {
              glColor3f(0.855, 0.647, 0.125);
            }
            strip_counter = strip_width;
          }
        }
         Vertex(th, ph);
         Vertex(th, ph+d);
         strip_counter--;
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0; th<=360; th+=d) {
      Vertex(th, 90-d);
   }
   glEnd();

   glPopMatrix();
}

// ====================== CIRCLE
void drawCircle(int num_segments, float cx, float cy, float r) {
  for(int ii=0; ii<num_segments; ii++) {
		float theta = 2.0f * 3.1415926f * ii / num_segments; //Current angle
		float x = r * cosf(theta); // Calculate the x component
		float y = r * sinf(theta); // Calculate the y component
    glVertex3f(x+cx, y+cy, 0);
	}
}

void circle(float x, float y, float z,
            float x_scalar, float y_scalar,
            float elevation, float azimuth, float theta, int num_segments) {

  float cx = 0.0;
  float cy = 0.0;
  float r = 1.0;

  glPushMatrix();

  glTranslatef(x, y, z);
  glRotatef(azimuth,1,0,0);
  glRotatef(elevation,0,1,0);
  glRotatef(theta,0,0,1);
  glScalef(x_scalar, y_scalar, 1.0);

  glColor4f(0.855, 0.647, 0.125, 0.4);
  glBegin(GL_POLYGON);
  drawCircle(num_segments, cx, cy, r);
  glEnd();

  glPopMatrix();
}

/* ====================== OBJECTS ====================== */
void ground(float y, float scale) {
  glPushMatrix();
  glTranslatef(0, y, 0);
  glScalef(scale, 0, scale);

  glColor3f(0.180, 0.545, 0.341);
  glBegin(GL_QUADS);
  // Front
  glVertex3f(-0.5, 0, -0.5);
  glVertex3f(-0.5, 0, 0.5);
  glVertex3f(0.5, 0, 0.5);
  glVertex3f(0.5, 0, -0.5);
  glEnd();
  glPopMatrix();
}

void bee(float x, float y, float z,
        float azimuth, float elevation, float theta,
        float scale,
        float head_z,
        float x_scalar, float y_scalar, float z_scalar) {
  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(azimuth,1,0,0);
  glRotatef(elevation,0,1,0);
  glRotatef(theta,0,0,1);
  glScalef(scale, scale, scale);

  // Head: x, y, z, r, x_scalar, y_scalar, z_scalar, head
  sphere(0,0,head_z, 0.1, 1,1,1, true);

  // Body: x, y, z, r, x_scalar, y_scalar
  sphere(0,0,0, 0.3, x_scalar,y_scalar,z_scalar, false);

  // Antennae: x,y,z, dx,dy,dz, az,elev,th
  cube(-0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,15);
  cube(0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,-15);

  // Wings
  glPushMatrix();
  glRotatef(60, 0,0,1);
  circle(-0.05,0.25,0.1, 0.1,0.2, 90,0,0, 50);
  glPopMatrix();

  glPushMatrix();
  glRotatef(-60, 0,0,1);
  circle(0.05,0.25,0.1, 0.1,0.2, 90,0,0, 50);
  glPopMatrix();

  glPopMatrix();
}

/* ====================== DISPLAY ====================== */
// OpenGL (GLUT) calls this routine to display the scene
void display() {
   const double len=1.5;  //  Length of axes

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the window and the depth buffer
   glEnable(GL_DEPTH_TEST); // Enable Z-buffering in OpenGL

   //  Undo previous transformations
   glLoadIdentity();

   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);

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

   // Drone bees: change azimuth
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
   glWindowPos2i(5,25);
   //  Print the text string
   Print("Angle=%d,%d",th,ph);
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

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
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// GLUT calls this routine when a key is pressed
void key(unsigned char ch,int x,int y) {
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// GLUT calls this routine when the window is resized
void reshape(int width,int height) {
   const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

// GLUT calls this toutine when there is nothing else to do
void idle() {
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t, 360);
   glutPostRedisplay();
}

// Start up GLUT and tell it what to do
int main(int argc,char* argv[]) {
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Objects");
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
