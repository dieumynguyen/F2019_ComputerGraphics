/*
 *  More Lighting
 *
 *  Demonstrates spotlights, lighting near large objects, two sided lighting
 *  and similar advanced lighting techniques using GLFW.
 *
 *  Key bindings:
 *  l/L        Toggle lighting on/off
 *  t/T        Toggle textures on/off
 *  p/P        Toggle projection between orthogonal/perspective
 *  b/B        Toggle display of quads
 *  +/-        Increase/decrease number of quads
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode on/off
 *  F3         Toggle two sided mode on/off
 *  'i'        Toggle light at infinity
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  []         Decrease/increase light elevation
 *  {}         Decrease/increase spot cutoff
 *  1/!        Decrease/increase constant attenuation
 *  2/@        Decrease/increase linear attenuation
 *  3/#        Decrease/increase quadratic attenuation
 *  x/X        Decrease/increase light X-position
 *  y/Y        Decrease/increase light Y-position
 *  z/Z        Decrease/increase light Z-position
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "CSCIx229.h"
int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int side=0;       //  Two sided mode
int ntex=1;       //  Texture mode
int th=0,ph=0;    //  View angles
int Th=0,Ph=30;   //  Light angles
float sco=180;    //  Spot cutoff angle
float Exp=0;      //  Spot exponent
int at0=100;      //  Constant  attenuation %
int at1=0;        //  Linear    attenuation %
int at2=0;        //  Quadratic attenuation %
int fov=53;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=8;     //  Size of world
// Light values
int num       =   1;  // Number of quads
int inf       =   0;  // Infinite distance light
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =   0;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
float X       = 0;    // Light X position
float Y       = 0;    // Light Y position
float Z       = 1;    // Light Z position

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   glVertex3d(Sin(th)*Cos(ph),Cos(th)*Cos(ph),Sin(ph));
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  Bands of latitude
   glColor3f(1,1,0);
   for (ph=-90;ph<90;ph+=10)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=20)
      {
         Vertex(th,ph);
         Vertex(th,ph+10);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  OpenGL calls this routine to display the scene
 */
void display(GLFWwindow* window)
{
   int i,j;
   const double len=2.0;  //  Length of axes
   double mul = 2.0/num;
   float Position[] = {X+Cos(Th),Y+Sin(Th),Z,1-inf};
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   glShadeModel(smooth?GL_SMOOTH:GL_FLAT);
   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Spotlight color and direction
      float yellow[] = {1.0,1.0,0.0,1.0};
      float Direction[] = {Cos(Th)*Sin(Ph),Sin(Th)*Sin(Ph),-Cos(Ph),0};
      //  Draw light position as ball (still no lighting here)
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  Two sided mode
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,side);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Set specular colors
      glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
      glMaterialf(GL_FRONT,GL_SHININESS,shiny);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
      //  Set spotlight parameters
      glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,Direction);
      glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,sco);
      glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,Exp);
      //  Set attenuation
      glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION ,at0/100.0);
      glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION   ,at1/100.0);
      glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,at2/100.0);
   }
   else
      glDisable(GL_LIGHTING);
   //  Enable textures
   if (ntex)
      glEnable(GL_TEXTURE_2D);
   else
      glDisable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   //  Draw the wall
   glColor3f(1.0,1.0,1.0);
   glNormal3f(0,0,1); 
   glBegin(GL_QUADS);
   for (i=0;i<num;i++)
      for (j=0;j<num;j++)
      {
         glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex2d(5*mul*(i+0)-5,5*mul*(j+0)-5);
         glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex2d(5*mul*(i+1)-5,5*mul*(j+0)-5);
         glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex2d(5*mul*(i+1)-5,5*mul*(j+1)-5);
         glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex2d(5*mul*(i+0)-5,5*mul*(j+1)-5);
      }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
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
      //  Show quads
      glBegin(GL_LINES);
      for (i=0;i<=num;i++)
      {
         glVertex3d(5*mul*i-5,-5,0.01);
         glVertex3d(5*mul*i-5,+5,0.01);
      }
      for (j=0;j<=num;j++)
      {
         glVertex3d(-5,5*mul*j-5,0.01);
         glVertex3d(+5,5*mul*j-5,0.01);
      }
      glEnd();
   }
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Projection=%s Light=%s",
     th,ph,dim,mode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,65);
      Print("Cutoff=%.0f Exponent=%.1f Direction=%d,%d Attenuation=%.2f,%.2f,%.2f",sco,Exp,Th,Ph,at0/100.0,at1/100.0,at2/100.0);
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s TwoSided=%s Position=%.1f,%.1f,%.1f,%.1f Num=%d",smooth?"Smooth":"Flat",local?"On":"Off",side?"On":"Off",Position[0],Position[1],Position[2],Position[3],num);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

/*
 *  Call this routine when a key is pressed
 */
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
     glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key==GLFW_KEY_0)
      X = Y = th = ph = 0;
   //  Toggle axes
   else if (key==GLFW_KEY_B)
      axes = 1-axes;
   //  Toggle textures
   else if (key==GLFW_KEY_T)
      ntex = 1-ntex;
   //  Toggle lighting
   else if (key==GLFW_KEY_L)
      light = 1-light;
   //  Toggle infinity
   else if (key==GLFW_KEY_I)
      inf = 1-inf;
   //  Switch projection mode
   else if (key==GLFW_KEY_P)
      mode = 1-mode;
   //  Change number of quadrangles
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && num>1)
      num--;
   else if ((key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL) && num<100)
      num++;
   //  Increase/decrease spot azimuth
   else if (key==GLFW_KEY_LEFT_BRACKET && !shift)
      Ph -= 5;
   else if (key==GLFW_KEY_RIGHT_BRACKET && !shift)
      Ph += 5;
   //  Increase/decrease spot cutoff angle
   else if (key==GLFW_KEY_LEFT_BRACKET && shift && sco>5)
      sco = sco==180 ? 90 : sco-5;
   else if (key==GLFW_KEY_RIGHT_BRACKET && shift && sco<180)
      sco = sco<90 ? sco+5 : 180;
   //  Change spot exponent
   else if (key==GLFW_KEY_COMMA)
   {
      Exp -= 0.1;
      if (Exp<0) Exp=0;
   }
   else if (key==GLFW_KEY_PERIOD)
      Exp += 0.1;
   //  Change constant attenuation
   else if (key==GLFW_KEY_1 && !shift && at0>0)
      at0--;
   else if (key==GLFW_KEY_1 && shift)
      at0++;
   //  Change linear attenuation
   else if (key==GLFW_KEY_2 && !shift && at1>0)
      at1--;
   else if (key==GLFW_KEY_2 && shift)
      at1++;
   //  Change quadratic attenuation
   else if (key==GLFW_KEY_3 && !shift && at2>0)
      at2--;
   else if (key==GLFW_KEY_3 && shift)
      at2++;
   //  Light position
   else if (key==GLFW_KEY_X && !shift)
      X -= 0.1;
   else if (key==GLFW_KEY_X && shift)
      X += 0.1;
   else if (key==GLFW_KEY_Y && !shift)
      Y -= 0.1;
   else if (key==GLFW_KEY_Y && shift)
      Y += 0.1;
   else if (key==GLFW_KEY_Z && !shift)
      Z -= 0.1;
   else if (key==GLFW_KEY_Z && shift)
      Z += 0.1;
   //  Ambient level
   else if (key==GLFW_KEY_A && !shift && ambient>0)
      ambient -= 5;
   else if (key==GLFW_KEY_A && shift && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (key==GLFW_KEY_D && !shift && diffuse>0)
      diffuse -= 5;
   else if (key==GLFW_KEY_D && shift && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (key==GLFW_KEY_S && !shift && specular>0)
      specular -= 5;
   else if (key==GLFW_KEY_S && shift && specular<100)
      specular += 5;
   //  Emission level
   else if (key==GLFW_KEY_E && !shift && emission>0)
      emission -= 5;
   else if (key==GLFW_KEY_E && shift && emission<100)
      emission += 5;
   //  Shininess level
   else if (key==GLFW_KEY_N && !shift && shininess>-1)
      shininess -= 1;
   else if (key==GLFW_KEY_N && shift && shininess<7)
      shininess += 1;
   //  Increase/decrease asimuth
   else if (key==GLFW_KEY_RIGHT)
      th += 5;
   else if (key==GLFW_KEY_LEFT)
      th -= 5;
   //  Increase/decrease elevation
   else if (key==GLFW_KEY_UP)
      ph += 5;
   else if (key==GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (key==GLFW_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key==GLFW_KEY_F2)
      local = 1-local;
   //  Two sided mode
   else if (key==GLFW_KEY_F3)
      side = 1-side;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Wrap angles
   Th %= 360;
   Ph %= 360;
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Call this routine when the window is resized
 */
void reshape(GLFWwindow* window,int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 * GLFW errors
 */
void error(int error, const char* text)
{
    fprintf(stderr,"Error: %s\n",text);
}

/*
 *  Start up GLFW and tell it what to do
 */
int main(int argc,char* argv[])
{
   int width,height;
   GLFWwindow* window;

   //  Initialize GLFW
   if (!glfwInit()) Fatal("Cannot initialize glfw\n");
   //  Error callback
   glfwSetErrorCallback(error);
   //  Set window properties
   glfwWindowHint(GLFW_RESIZABLE,1);
   glfwWindowHint(GLFW_DOUBLEBUFFER,1);
   //  Create window and make current
   window = glfwCreateWindow(600, 600, "More Lighting - GLFW", NULL, NULL);
   if (!window) Fatal("Cannot create GLFW window\n");
   glfwMakeContextCurrent(window);
   //  Enable VSYNC
   glfwSwapInterval(1);
   //  Set callback for window resize
   glfwSetWindowSizeCallback(window,reshape);
   //  Set initial window size
   glfwGetWindowSize(window,&width,&height);
   reshape(window,width,height);
   //  Set callback for keyboard input
   glfwSetKeyCallback(window,key);

   //  Load textures
   LoadTexBMP("brick.bmp");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Elapsed time in seconds
      double t = glfwGetTime();
      Th = fmod(90*t,360.0);
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
