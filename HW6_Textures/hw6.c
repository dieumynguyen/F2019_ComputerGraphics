#include <stdio.h>
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

// Macro for sin & cos in degrees
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

/* ====================== GLOBALS ====================== */
int th = 0;           // Azimuth
int ph = 10;          // Elevation
double zh = 90;       // Rotation
int axes = 1;         // Display axes

// For projections
int firstPerson = 0;  // First person mode is perspective
int projectionMode = 1;         // Projection mode: 0 for orthographic, 1 for perspective
int fov = 55;         // Field of view for perspective
double asp = 1;       // Aspect ratio
double dim = 3.0;     // Size of world; 4
// For FP Perspective
float xPosition = 0, yPosition = 0, zPosition = 4;  // Position of camera
float xRotation = 0, yRotation = 0, zRotation = 0;  // Rotation axes
float angle = 0.0;									// Angle of rotation
float cRadius = 4.0;                   				// Our distance from the camera
float xLast, yLast; 								// Previous x,y

// For lighting
int moveLight = 1;
int light = 1;
// Light values
int one = 1;       // Unit value
int distance = 5;  // Light distance
int inc = 5;      // Ball increment
int smooth = 1;    // Smooth/Flat shading
int local = 0;     // Local Viewer Model
int emission = 0;  // Emission intensity (%)
int ambient = 50;  // Ambient intensity (%)
int diffuse = 100; // Diffuse intensity (%)
int specular = 0;  // Specular intensity (%)
int shininess = 0; // Shininess (power of two)
float shiny = 1;   // Shininess (value)
float ylight = 0;  // Elevation of light

// Textures
int textureMode = 0;
unsigned int texture[2]; // Texture names

/* ====================== PRINT ====================== */
#define LEN 8192  // Maximum length of text string
void Print(const char* format, ...)
{
   char buf[LEN];
   char* ch = buf;
   va_list args;
   // Turn the parameters into a character string
   va_start(args, format);
   vsnprintf(buf, LEN, format, args);
   va_end(args);
   // Display the characters one at a time at the current raster position
   while (*ch)
   {
       glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *ch++);
   }
}
/* ====================== ERROR CHECK ====================== */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}
/* ====================== TEXTURE LOADING ====================== */
static void Reverse(void* x,const int n)
{
   int k;
   char* ch = (char*)x;
   for (k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}
unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   off;        // Image offset
   unsigned int   k;          // Counter
   int            max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Read header
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&off,4);
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}
/* ====================== SHAPES ====================== */
// ========== CUBOID
void cube(double x, double y, double z,
          double dx, double dy, double dz,
          double azimuth, double elevation, double theta)
{
  // Save transformation
  glPushMatrix();

  // Transform cube
  glTranslatef(x,y,z);
  glRotatef(azimuth,1,0,0);
  glRotatef(elevation,0,1,0);
  glRotatef(theta,0,0,1);
  glScalef(dx,dy,dz);

  // Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Brown color
  glColor3f(0.545, 0.271, 0.075);

  // Make cube
  glBegin(GL_QUADS);
  // Front
  glNormal3f( 0, 0, 1);
  glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
  glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
  glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
  glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
  // Back
  // glBindTexture(GL_TEXTURE_2D,texture[0]);
  glNormal3f( 0, 0,-1);
  glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
  glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
  // Right
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
  glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
  // Left
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
  glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
  glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  // Top
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
  glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
  glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  // Bottom
  glNormal3f( 0,-1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
  glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
  // End
  glEnd();
  // Undo transofrmations
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

// ========== SPHERE
void Vertex(double th, double ph)
{
   double x = Sin(th) * Cos(ph);
   double y = Cos(th) * Cos(ph);
   double z = Sin(ph);
   // For a sphere at the origin, the position & normal vectors are the same
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0, ph/180.0+0.5);
   glVertex3d(x,y,z);
}

void sphere(double x, double y, double z, double r,
                   double x_scalar, double y_scalar, double z_scalar,
                   bool head)
{
    const int inc = 2;
    int th, ph;

    glPushMatrix();

    // Offset and scale
    glTranslated(x, y, z);
    glScaled(x_scalar*r, y_scalar*r, z_scalar*r);

    // Default brown color
    glColor3f(0.545, 0.271, 0.075);

    // Texture
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    int strip_width = 6;
    int strip_counter = strip_width;
    int color_idx = 0;

    // Bands of latitude
    for (ph=-90; ph<90; ph+=inc)
    {
      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=2*inc)
      {
         if (!head) {
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
         Vertex(th, ph+inc);
      }
      glEnd();
      strip_counter--;
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// ========== WING
void wing(double x, double y, double z, double r,
                 double x_scalar, double y_scalar, double z_scalar)
{
    int inc = 5;
    int th, ph;
    // Save transformation
    glPushMatrix();
    // Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(x_scalar*r, y_scalar*r, z_scalar*r);

    // Set transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Color
    glColor4f(0.855, 0.647, 0.125, 0.3);
    // Bands of latitude
    for (ph=-90; ph<90; ph+=inc){
        glBegin(GL_QUAD_STRIP);
        for (th=0; th<=360; th+=2*inc) {
            Vertex(th, ph);
            Vertex(th, ph+inc);
        }
        glEnd();
    }
    // Undo transofrmations
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
}

/* ====================== OBJECTS ====================== */
void ground(float y, float scale)
{
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
        float scale, float head_z,
        float x_scalar, float y_scalar, float z_scalar)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(azimuth, 1, 0, 0);
    glRotatef(elevation, 0, 1, 0);
    glRotatef(theta, 0, 0, 1);
    glScalef(scale, scale, scale);

    // Head: x, y, z, r, x_scalar, y_scalar, z_scalar, head
    sphere(0,0,head_z, 0.1, 1,1,1, true);

    // Body: x, y, z, r, x_scalar, y_scalar
    sphere(0,0,0, 0.3, x_scalar,y_scalar,z_scalar, false);

    // Antennae: x,y,z, dx,dy,dz, az,elev,th
    // glColor3f(0.855, 0.647, 0.125);
    cube(-0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,15);
    cube(0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,-15);

    // Wings: x,y,z, r, x_scalar,y_scalar,z_scalar
    // Wing 1
    glPushMatrix();
    glRotatef(60, 0,0,1);
    wing(-0.05,0.25,0.1, 0.3, 0.1,0.8,0.4);
    glPopMatrix();
    // Wing 2
    glPushMatrix();
    glRotatef(-60, 0,0,1);
    wing(0.05,0.25,0.1, 0.3, 0.1,0.8,0.4);
    glPopMatrix();

    glPopMatrix();
}

/* ====================== LIGHT AS A BALL ====================== */
void ball(double x, double y, double z, double r)
{
    int th,ph;
    float yellow[] = {1.0, 1.0, 0.0, 1.0};
    float Emission[]  = {0.0, 0.0, 0.01*emission, 1.0};
    // Save transformation
    glPushMatrix();
    // Offset, scale and rotate
    glTranslated(x, y, z);
    glScaled(r, r, r);
    // White ball
    glColor3f(1, 1, 1);
    glMaterialf(GL_FRONT, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
    // Bands of latitude
    for (ph=-90; ph<90; ph+=inc)
    {
      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=2*inc)
      {
         Vertex(th, ph);
         Vertex(th, ph+inc);
      }
      glEnd();
    }
    // Undo transofrmations
    glPopMatrix();
}

/* ====================== DISPLAY ====================== */
// OpenGL (GLUT) calls this routine to display the scene
void display()
{
    const double len = 1.5;  //  Length of axes
    // Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    // Undo previous transformations
    glLoadIdentity();

    // Perspective - set eye position
    if (projectionMode && !firstPerson) {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim*Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
    }
	// First person in perspective
    else if (projectionMode && firstPerson) {
        glTranslatef(0.0, 0.0, -cRadius);
        glRotatef(xRotation, 1.0, 0.0, 0.0);
        // Our cube character to follow
        // glColor3f(0.502, 0.502, 0.502);
        // cube(0,0,0, 0.2,0.2,0.2, 0,0,0);
        // Rotate camera on y-axis, up/down
        glRotatef(yRotation, 0.0, 1.0, 0.0);
        // Translate screen to the position of camera
        glTranslated(-xPosition, 0.0f, -zPosition);
    }
    // Orthogonal - set world orientation
    else {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }

    /* ========= SET UP LIGHTING ========= */
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    // Light switch
    if (light) {
        // Translate intensity to color vectors
        float Ambient[] = {0.01*ambient, 0.01*ambient, 0.01*ambient, 1.0};
        float Diffuse[] = {0.01*ambient, 0.01*ambient, 0.01*ambient, 1.0};
        float Specular[] = {0.01*ambient, 0.01*ambient, 0.01*ambient, 1.0};
        // Light position
        float Position[] = {distance*Cos(zh), ylight, distance*Sin(zh), 1.0};
        // Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0], Position[1], Position[2], 0.1);
        // Tell opengl to normalize normal vectors
        glEnable(GL_NORMALIZE);
        // Enable lighting
        glEnable(GL_LIGHTING);
        // Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
        // glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        // Enable light 0
        glEnable(GL_LIGHT0);
        // Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT, Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR, Specular);
        glLightfv(GL_LIGHT0,GL_POSITION, Position);
    }
    else {
        glDisable(GL_LIGHTING);
    }

    /* ========= CALL SHAPES HERE ========= */
    // Bee: x, y, z, azimuth, elevation, theta, scale, head_z, x_scalar, y_scalar, z_scalar
    // Queen bee
    bee(0,0,0, 0,0,0, 0.7, 0.35,0.3,0.3,1.5);

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

    // Draw axes - no lighting from here on
    glDisable(GL_LIGHTING);
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
        // Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }
    // Display parameters
    glWindowPos2i(5,5);
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
          th,ph,dim,fov,projectionMode?"Perpective":"Orthogonal",light?"On":"Off");
    if (light) {
        glWindowPos2i(5,45);
        Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
        glWindowPos2i(5,25);
        Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
    }
    // Render the scene and make it visible
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
    angle++;
}

/* ====================== PROJECTIONS ====================== */
static void Project()
{
    // Tell opengl we want to manipulate projection matrix
    glMatrixMode(GL_PROJECTION);
    // Undo previous transformations
    glLoadIdentity();
    // Projection transformation
    // If first person, always in perspective
    if (projectionMode) {
        // gluPerspective(fov, asp, dim/4, 4*dim);
        float nearPlane = dim / 500.0;
        float farPlane = 200 * dim;
        gluPerspective(fov, asp, nearPlane, farPlane);
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
void special(int key, int x, int y)
{
    // Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT) {
        th += 5;
        // yRotation += 5;
    }
    // Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT) {
        th -= 5;
        // yRotation -= 5;
    }
    // Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP) {
        ph +=  5;
        // xRotation += 5;
    }
    // Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN) {
        ph -= 5;
        // xRotation -= 5;
    }
    // PageUp key - increase dim
    else if (key == GLUT_KEY_PAGE_UP) {
        dim += 0.1;
    }
    // PageDown key - decrease dim
    else if (key == GLUT_KEY_PAGE_DOWN && dim>1) {
        dim -= 0.1;
    }
    // Smooth color model
    else if (key == GLUT_KEY_F1) {
        smooth = 1 - smooth;
    }
    // Local Viewer
    else if (key == GLUT_KEY_F2) {
        local = 1 - local;
    }
    else if (key == GLUT_KEY_F3) {
        distance = (distance==1) ? 5 : 1;
    }
    // Toggle ball increment
    else if (key == GLUT_KEY_F8) {
        inc = (inc==10)?3:10;
    }
    // Flip sign
    else if (key == GLUT_KEY_F9) {
        one = -one;
    }
    // Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    // Update projection
    Project();
    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

// GLUT calls this toutine when there is nothing else to do
void idle()
{
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    zh = fmod(90*t, 360);
    glutPostRedisplay();
}

// GLUT calls this routine when a key is pressed
void key(unsigned char key, int x, int y)
{
    // Exit on ESC
    if (key == 27) {
        exit(0);
    }
    // Reset view angle
    else if (key == '0') {
        th = ph = 0;
    }
    // Toggle axes
    else if (key == 'x' || key == 'X') {
        axes = 1 - axes;
    }

    // ====================== TEXTURE KEYS
    else if (key == 't' || key == 'T') {
       textureMode = 1 - textureMode;
    }

    // ====================== PROJECTION KEYS
    else if (key == 'p' || key == 'P') {
        projectionMode = 1 - projectionMode;
    }
    // Turn on/off firstPerson mode
    else if (key == 'f' || key == 'F') {
        firstPerson = 1 - firstPerson;
    }
    // Change field of view angle
    else if (key == '-' && key > 1) {
        fov--;
    }
    else if (key == '+' && key < 179) {
        fov++;
    }
    // Go up
    if (key == 'u') {
        xRotation += 1;
        if (xRotation > 360) {
            xRotation -= 360;
      }
    }
    // Go down
    if (key == 'm') {
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
    if (key == 's') {
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
        xPosition += (float)cos(yRotRad) * 0.1;
        zPosition += (float)sin(yRotRad) * 0.1;
    }
    // Go left
    if (key == 'a') {
        float yRotRad;
        yRotRad = (yRotation / 180 * 3.141592654f);
        xPosition -= (float)cos(yRotRad) * 0.1;
        zPosition -= (float)sin(yRotRad) * 0.1;
    }

    // ====================== LIGHTING KEYS
    // Toggle lighting
    else if (key == 'l' || key == 'L') {
        light = 1 - light;
    }
    // Toggle light movement
    else if (key =='m' || key == 'M') {
        moveLight = 1 - moveLight;
    }
    // Move light
    else if (key == '(') {
        zh += 1;
    }
    else if (key == ')') {
        zh -= 1;
    }
    // Light elevation
    else if (key=='[') {
        ylight -= 0.1;
    }
    else if (key==']') {
        ylight += 0.1;
    }
    // Ambient level
    else if (key=='i' && ambient>0) {
        ambient -= 5;
    }
    else if (key=='I' && ambient<100) {
        ambient += 5;
    }
    // Diffuse level
    else if (key=='u' && diffuse>0) {
        diffuse -= 5;
    }
    else if (key=='U' && diffuse<100) {
        diffuse += 5;
    }
    // Specular level
    else if (key=='o' && specular>0) {
        specular -= 5;
    }
    else if (key=='O' && specular<100) {
        specular += 5;
    }
    // Emission level
    else if (key=='e' && emission>0) {
        emission -= 5;
    }
    else if (key=='E' && emission<100) {
        emission += 5;
    }
    // Shininess level
    else if (key=='n' && shininess>-1) {
        shininess -= 1;
    }
    else if (key=='N' && shininess<7) {
        shininess += 1;
    }
    // Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);

    // Reproject
    Project();
    // Animate if requested
    glutIdleFunc(moveLight?idle:NULL);
    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

void mouseMovement(int x, int y)
{
    // Check diff bw current and last x position
    int xDiff = x - xLast;
    // Check diff bw current and last y position
    int yDiff = y - yLast;
    // Set last x, y position to current x, y position
    xLast = x;
    yLast = y;
    // Set x rotation with addition of difference in y position
    xRotation += (float) yDiff * 0.1;
    // Set the x rotation
    yRotation += (float) xDiff * 0.1;
}

/* ====================== HELPER FUNCTIONS ====================== */
// GLUT calls this routine when the window is resized
void reshape(int width,int height)
{
    // Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    // Set the viewport to the entire window
    glViewport(0, 0, width, height);
    // Set projection
    Project();
}

/* ====================== MAIN ====================== */
// Start up GLUT and tell it what to do
int main(int argc,char* argv[])
{
    // Initialize GLUT and process user parameters
    glutInit(&argc,argv);
    // Request double buffered, true color window with Z buffering at 600x600
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    // Create the window
    glutCreateWindow("Dieu My Nguyen - Textures");
    // Tell GLUT to call "idle" when there is nothing else to do
    glutIdleFunc(idle);
    // Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    // Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    // Check for mouse movement
    glutPassiveMotionFunc(mouseMovement);
    // Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    // Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    // Pass control to GLUT so it can interact with the user

    // Load textures
    texture[0] = LoadTexBMP("wing.bmp");
    texture[1] = LoadTexBMP("fuzzy.bmp");

    glutMainLoop();
    return 0;
}
