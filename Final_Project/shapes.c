#include "CSCIx229.h"

/* ====================== SHAPES ====================== */
// ========== CUBOID
void cube(double x, double y, double z,
          double dx, double dy, double dz,
          double azimuth, double elevation, double theta,
          int textureMode, unsigned int texture[])
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
           bool head, int textureMode, unsigned int texture[])
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
          double x_scalar, double y_scalar, double z_scalar,
          unsigned int texture[])
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
void ground(float x, float y, float z, float scale, double concentrationMap[301][301])
{

    // printf("Debug value: %f\n", concentrationMap[100][100]);

    // Save transformation
    glPushMatrix();

    // Transform cube
    glTranslatef(x,y,z);
    glScalef(scale, 0.05, scale);

    // Make cube
    glBegin(GL_QUADS);
    // Front
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    // Back
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
    // Bottom
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    glEnd();

    glUseProgram(1);

    // Top
    // ---------------------------------------------------------------
    // Disable lighting for top surface where pheromones are drawn
    glDisable(GL_LIGHTING);

    // Toggle drawing platform surface with 4 vertices or arenaSize vertices
    int simpleSurface = 0;
    if (simpleSurface) {
        // Draw with 4 vertices
        glBegin(GL_QUADS);
        glNormal3f( 0,+1, 0);
        glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
        glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
        glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
        glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
        glEnd();
    }
    else {
        // Draw with arenaSize number of vertices
        glPushMatrix();

        int nRows = 301;
        int nCols = 301;
        glTranslatef(-1,0,-1);
        glScalef(1/(float)nRows, 1, 1/(float)nCols);
        glScalef(2, 1, 2);

        // Draw fine surface
        int i, j, m, n;
        for (i=0; i < nRows-1; i++)
        {
            m = i+1;
            for (j=0; j < nCols-1; j++)
            {
                n = j+1;
                glBegin(GL_QUADS);

                // float v1[3] = {i, 1, j};
                // float v2[3] = {i, 0.5, n};
                // float v3[3] = {m, 0.25, n};
                // float v4[3] = {m, 0, j};

                double mapY = concentrationMap[i][j];

                float v1[3] = {i, mapY, j};
                float v2[3] = {i, mapY, n};
                float v3[3] = {m, mapY, n};
                float v4[3] = {m, mapY, j};

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(v2[0], v2[1], v2[2]);
                glVertex3f(v3[0], v3[1], v3[2]);
                glVertex3f(v4[0], v4[1], v4[2]);
                glEnd();
            }
        }
        glPopMatrix();
    }
    // No shader for what follows
    glUseProgram(0);
    // ---------------------------------------------------------------

    glEnable(GL_LIGHTING);
    // Undo transformations
    glPopMatrix();
}

void bee(float x, float y, float z,
        float azimuth, float elevation, float theta,
        float scale, float head_z,
        float x_scalar, float y_scalar, float z_scalar,
        int wing_angle,
        int textureMode, unsigned int texture[])
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(azimuth, 1, 0, 0);
    glRotatef(elevation, 0, 1, 0);
    glRotatef(theta, 0, 0, 1);
    glScalef(scale, scale, scale);

    // Head: x, y, z, r, x_scalar, y_scalar, z_scalar, head
    sphere(0,0,head_z, 0.1, 1,1,1, true, textureMode, texture);

    // Body: x, y, z, r, x_scalar, y_scalar
    sphere(0,0,0, 0.3, x_scalar,y_scalar,z_scalar, false, textureMode, texture);

    // Antennae: x,y,z, dx,dy,dz, az,elev,th
    // glColor3f(0.855, 0.647, 0.125);
    cube(-0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,15, textureMode, texture);
    cube(0.05,0.15,0.35, 0.01,0.2,0.01, 0,15,-15, textureMode, texture);

    // Wings: x,y,z, r, x_scalar,y_scalar,z_scalar
    // Wing 1
    glPushMatrix();
    // glRotatef(wing_angle, 1,0,0);
    glRotatef(wing_angle, 0,0,1);
    wing(-0.05,0.25,0.1, 0.3, 0.1,0.8,0.4, texture);
    // wing(-0.08,0,-0.05, 0.3, 0.1,0.8,0.4, texture);
    glPopMatrix();
    // Wing 2
    glPushMatrix();
    glRotatef(-wing_angle, 0,0,1);
    wing(0.05,0.25,0.1, 0.3, 0.1,0.8,0.4, texture);
    glPopMatrix();

    glPopMatrix();
}

/* ====================== LIGHT AS A BALL ====================== */
void ball(double x, double y, double z, double r,
          int emission, float shiny)
{
    int th,ph;
    int inc = 5;
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
