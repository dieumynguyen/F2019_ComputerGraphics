#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

#ifdef __cplusplus
extern "C" {
#endif

void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(const char* file);
void Project(int projectionMode, int fov, double asp, double dim);
void ErrCheck(const char* where);
// int  LoadOBJ(const char* file);

// Shapes stuff
void cube(double x, double y, double z,
          double dx, double dy, double dz,
          double azimuth, double elevation, double theta,
          int textureMode, unsigned int texture[]);
void Vertex(double th, double ph);
void sphere(double x, double y, double z, double r,
           double x_scalar, double y_scalar, double z_scalar,
           bool head, int textureMode, unsigned int texture[]);
void wing(double x, double y, double z, double r,
         double x_scalar, double y_scalar, double z_scalar,
         unsigned int texture[]);
void ground(float x, float y, float z, float scale, double concentrationMap[301][301]);
void bee(float x, float y, float z,
        float azimuth, float elevation, float theta,
        float scale, float head_z,
        float x_scalar, float y_scalar, float z_scalar,
        int wing_angle,
        int textureMode, unsigned int texture[]);
void ball(double x, double y, double z, double r,
        int emission, float shiny);


#ifdef __cplusplus
}
#endif

#endif
