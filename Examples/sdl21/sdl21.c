/*
 *  More Lighting
 *
 *  Demonstrates spotlights, lighting near large objects, two sided lighting
 *  and similar advanced lighting techniques using SDL.
 *
 *  Key bindings:
 *  space      Play boing sound
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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
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
Mix_Chunk* boing;     // boing sound

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
void display()
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
   SDL_GL_SwapBuffers();
}

/*
 *  Call this routine when a key is pressed
 *     Returns 1 to continue, 0 to exit
 */
int key()
{
   Uint8* keys = SDL_GetKeyState(NULL);
   int shift = SDL_GetModState()&KMOD_SHIFT;
   //  Exit on ESC
   if (keys[SDLK_ESCAPE])
      return 0;
   //  Reset view angle
   else if (keys[SDLK_0])
      X = Y = th = ph = 0;
   //  Toggle axes
   else if (keys[SDLK_b])
      axes = 1-axes;
   //  Toggle textures
   else if (keys[SDLK_t])
      ntex = 1-ntex;
   //  Toggle lighting
   else if (keys[SDLK_l])
      light = 1-light;
   //  Toggle infinity
   else if (keys[SDLK_i])
      inf = 1-inf;
   //  Switch projection mode
   else if (keys[SDLK_p])
      mode = 1-mode;
   //  Change number of quadrangles
   else if ((keys[SDLK_KP_MINUS] || keys[SDLK_MINUS]) && num>1)
      num--;
   else if ((keys[SDLK_KP_PLUS] || keys[SDLK_EQUALS]) && num<100)
      num++;
   //  Increase/decrease spot azimuth
   else if (keys[SDLK_LEFTBRACKET] && !shift)
      Ph -= 5;
   else if (keys[SDLK_RIGHTBRACKET] && !shift)
      Ph += 5;
   //  Increase/decrease spot cutoff angle
   else if (keys[SDLK_LEFTBRACKET] && shift && sco>5)
      sco = sco==180 ? 90 : sco-5;
   else if (keys[SDLK_RIGHTBRACKET] && shift && sco<180)
      sco = sco<90 ? sco+5 : 180;
   //  Change spot exponent
   else if (keys[SDLK_COMMA])
   {
      Exp -= 0.1;
      if (Exp<0) Exp=0;
   }
   else if (keys[SDLK_PERIOD])
      Exp += 0.1;
   //  Change constant attenuation
   else if (keys[SDLK_1] && !shift && at0>0)
      at0--;
   else if (keys[SDLK_1] && shift)
      at0++;
   //  Change linear attenuation
   else if (keys[SDLK_2] && !shift && at1>0)
      at1--;
   else if (keys[SDLK_2] && shift)
      at1++;
   //  Change quadratic attenuation
   else if (keys[SDLK_3] && !shift && at2>0)
      at2--;
   else if (keys[SDLK_3] && shift)
      at2++;
   //  Light position
   else if (keys[SDLK_x] && !shift)
      X -= 0.1;
   else if (keys[SDLK_x] && shift)
      X += 0.1;
   else if (keys[SDLK_y] && !shift)
      Y -= 0.1;
   else if (keys[SDLK_y] && shift)
      Y += 0.1;
   else if (keys[SDLK_z] && !shift)
      Z -= 0.1;
   else if (keys[SDLK_z] && shift)
      Z += 0.1;
   //  Ambient level
   else if (keys[SDLK_a] && !shift && ambient>0)
      ambient -= 5;
   else if (keys[SDLK_a] && shift && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (keys[SDLK_d] && !shift && diffuse>0)
      diffuse -= 5;
   else if (keys[SDLK_d] && shift && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (keys[SDLK_s] && !shift && specular>0)
      specular -= 5;
   else if (keys[SDLK_s] && shift && specular<100)
      specular += 5;
   //  Emission level
   else if (keys[SDLK_e] && !shift && emission>0)
      emission -= 5;
   else if (keys[SDLK_e] && shift && emission<100)
      emission += 5;
   //  Shininess level
   else if (keys[SDLK_n] && !shift && shininess>-1)
      shininess -= 1;
   else if (keys[SDLK_n] && shift && shininess<7)
      shininess += 1;
   //  Increase/decrease asimuth
   else if (keys[SDLK_RIGHT])
      th += 5;
   else if (keys[SDLK_LEFT])
      th -= 5;
   //  Increase/decrease elevation
   else if (keys[SDLK_UP])
      ph += 5;
   else if (keys[SDLK_DOWN])
      ph -= 5;
   //  PageUp key - increase dim
   else if (keys[SDLK_PAGEDOWN])
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (keys[SDLK_PAGEUP] && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (keys[SDLK_F1])
      smooth = 1-smooth;
   //  Local Viewer
   else if (keys[SDLK_F2])
      local = 1-local;
   //  Two sided mode
   else if (keys[SDLK_F3])
      side = 1-side;
   //  Play boing once
   else if (keys[SDLK_SPACE])
      Mix_PlayChannel(-1,boing,0);
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Wrap angles
   Th %= 360;
   Ph %= 360;
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
   //  Return 1 to keep running
   return 1;
}

/*
 *  Call this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Start up SDL and tell it what to do
 */
int main(int argc,char* argv[])
{
   int run=1;
   double t0=0;
   Mix_Music* music;
   SDL_Surface* screen;

   //  Initialize SDL
   SDL_Init(SDL_INIT_VIDEO);
   //  Set size, resizable and double buffering
   screen = SDL_SetVideoMode(600,600,0,SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
   if (!screen ) Fatal("Cannot set SDL video mode\n");
   //  Set window and icon labels
   SDL_WM_SetCaption("More Lighting - SDL","sdl21");
   //  Set screen size
   reshape(screen->w,screen->h);

   //  Load textures
   LoadTexBMP("brick.bmp");

   //  Initialize audio
   if (Mix_OpenAudio(44100,AUDIO_S16SYS,2,4096)) Fatal("Cannot initialize audio\n");
   //  Load "The Wall"
   music = Mix_LoadMUS("thewall.ogg");
   if (!music) Fatal("Cannot load thewall.ogg\n");
   //  Load boing
   boing = Mix_LoadWAV("boing.wav");
   if (!boing) Fatal("Cannot load boing.wav\n");
   //  Play (looping)
   Mix_PlayMusic(music,-1);

   //  SDL event loop
   ErrCheck("init");
   while (run)
   {
      //  Elapsed time in seconds
      double t = SDL_GetTicks()/1000.0;
      //  Process all pending events
      SDL_Event event;
      while (SDL_PollEvent(&event))
         switch (event.type)
         {
            case SDL_VIDEORESIZE:
               screen = SDL_SetVideoMode(event.resize.w,event.resize.h,0,SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
               reshape(screen->w,screen->h);
               break;
            case SDL_QUIT:
               run = 0;
               break;
            case SDL_KEYDOWN:
               run = key();
               t0 = t+0.5;  // Wait 1/2 s before repeating
               break;
            default:
               //  Do nothing
               break;
         }
      //  Repeat key every 50 ms
      if (t-t0>0.05)
      {
         run = key();
         t0 = t;
      }
      //  Display
      Th = fmod(90*t,360.0);
      display();
      //  Slow down display rate to about 100 fps by sleeping 5ms
      SDL_Delay(5);
   }
   //  Shut down SDL
   Mix_CloseAudio();
   SDL_Quit();
   return 0;
}
