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

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

/* ====================== SHAPES ====================== */
// ========== CUBOID
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

  // glColor3f(0.855, 0.647, 0.125);
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

// ========== SPHERE
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

// ========== CIRCLE
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
  glColor3f(0.855, 0.647, 0.125);
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
