#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/************** GLOBAL VARIABLES **************/
int th = 0;
int ph = 0;
double z = 0;
double dim = 2;

// Lorenz parameters
double s = 10;
double b = 2.6666;
double r = 28;

// Time steps and save lorenz points
int num_timesteps = 50000;
double lorenz_points[50000][3];

// Time counter for animation of moving point
int time_step = 0;

/************** LORENZ FUNCTIONS **************/
void normalize_array(double array[], int num_elements) {
  // Get min and max of array
  double array_min = array[0];
  double array_max = array[0];
  for (int i=0; i<num_elements; i++) {
    // Find min
    if (array[i] < array_min) {
      array_min = array[i];
    }
    // Find max
    if (array[i] > array_max) {
      array_max = array[i];
    }
  }
  // Loop over each element in array and normalize to -1 to 1 for OpenGL
  for (int i=0; i<num_elements; i++) {
    double x = array[i];
    double new_x = (((x - array_min) / (array_max - array_min)) * 2) - 1;
    // Replace old x with normalized x
    array[i] = new_x;
  }
}

void generate_lorenz(void) {
  int i;
  // Coordinates
  double x = 1;
  double y = 1;
  double z = 1;

  // Time step
  double dt = 0.001;

  // Lists for single dimensions to keep track of unnormalized values
  double xs[num_timesteps];
  double ys[num_timesteps];
  double zs[num_timesteps];

  // Integrate 50,000 steps (50 time units with dt = 0.001)
  // xplicit Euler integration
  for (i=0; i<num_timesteps; i++) {
     double dx = s*(y-x);
     double dy = x*(r-z)-y;
     double dz = x*y - b*z;
     x += dt*dx;
     y += dt*dy;
     z += dt*dz;

     // Add unnormalized to single lists
     xs[i] = x;
     ys[i] = y;
     zs[i] = z;
  }

  // Normalize each single list
  normalize_array(xs, num_timesteps);
  normalize_array(ys, num_timesteps);
  normalize_array(zs, num_timesteps);

  // Store normalized data
  for (int i=0; i<num_timesteps; i++) {
    lorenz_points[i][0] = xs[i];
    lorenz_points[i][1] = ys[i];
    lorenz_points[i][2] = zs[i];
  }
}

/************** HELPER & OPENGL FUNCTIONS **************/
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...) {
  char buf[LEN];
  char* ch=buf;
  va_list args;

  //  Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);

  //  Display the characters one at a time at the current raster position
  while (*ch)
  glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*ch++);
}

void key(unsigned char k, int x, int y) {
  // Exist on ESC
  if (k == 27)
    exit(0);
  // Reset view angle
  else if (k == '0')
    th = ph = 0;
  // Change parameters of Lorenz attractor
  else if (k == 'r') {
    s = 10;
    b = 2.666;
    r = 28;
  }
  // Increase/decrease sigma
  else if (k == 'a')
    s -= 1;
  else if (k == 'q')
    s += 1;
  // Increase/decrease beta
  else if (k == 'd')
    b -= 0.1;
  else if (k == 'e')
    b += 0.1;
  // Increase/decrease rho
  else if (k == 'g')
    r -= 1;
  else if (k == 't')
    r += 1;
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

// GLUT calls this routine when the window is resized
void reshape(int width,int height) {
  //  Ratio of the width to the height of the window
  double w2h = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Orthogonal projection box adjusted for the
  //  aspect ratio of the window
  glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

// GLUT calls this routine when an arrow key is pressed
void special(int key,int x,int y) {
  //  Right arrow key - increase azimuth by 5 degrees
  if (key == GLUT_KEY_RIGHT)
    th += 5;
  //  Left arrow key - decrease azimuth by 5 degrees
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

void display() {
  // Make the attractor, save all points
  generate_lorenz();

  // Clear image
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Reset previous transforms
  glLoadIdentity();

  // Set view angle
  glRotated(ph,1,0,0);
  glRotated(th,0,1,0);

  // Plot line that makes up attractor
  int i;
  glBegin(GL_LINE_STRIP);
  for (i=0; i<num_timesteps; i++) {
    glColor3f(0.686, 0.933, 0.933);
    glVertex3dv(lorenz_points[i]);
  }
  glEnd();

  // Draw a point that goes around attractor
  glColor3f(0.957, 0.643, 0.376);
  glPointSize(7);
  glBegin(GL_POINTS);
  int step_size = 100;
  int step = time_step + step_size;
  float point_x = lorenz_points[step][0];
  float point_y = lorenz_points[step][1];
  float point_z = lorenz_points[step][2];
  glVertex3d(point_x, point_y, point_z);
  glEnd();
  time_step ++;
  time_step = time_step % num_timesteps;

  //  Draw axes in white
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3d(0,0,0);
  glVertex3d(1,0,0);
  glVertex3d(0,0,0);
  glVertex3d(0,1,0);
  glVertex3d(0,0,0);
  glVertex3d(0,0,1);
  glEnd();

  //  Label axes
  glRasterPos3d(1,0,0);
  Print("X");
  glRasterPos3d(0,1,0);
  Print("Y");
  glRasterPos3d(0,0,1);
  Print("Z");

  //  Display parameters
  glWindowPos2i(5,5);
  Print("View Angle=%d, %d", th, ph);
  glWindowPos2i(5,470);
  Print("Parameters:");
  glWindowPos2i(5,450);
  Print("Sigma: %0.0f, Beta: %0.2f, Rho: %0.0f", s, b, r);
  glWindowPos2i(5,410);
  Print("Keys:");
  glWindowPos2i(5,390);
  Print("q: + sigma, a: - sigma");
  glWindowPos2i(5,370);
  Print("e: + beta, d: - beta");
  glWindowPos2i(5,350);
  Print("t: + rho, g: - rho");

  //  Flush and swap
  glFlush();
  glutSwapBuffers();
}

/************** MAIN PROGRAM **************/
int main(int argc,char* argv[]) {
  // Initialize GLUT and process user parameters
  glutInit(&argc,argv);
  // Request double buffered, true color window
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition(50, 50);
  // Request window of size
  glutInitWindowSize(700, 500);
  // Create the window
  glutCreateWindow("Dieu My Nguyen - Lorenz Attractor");
  // Tell GLUT to call "display" when the scene should be drawn
  glutDisplayFunc(display);
  // Tell GLUT to call "reshape" when the window is resized
  glutReshapeFunc(reshape);
  // Tell GLUT to call "special" when an arrow key is pressed
  glutSpecialFunc(special);
  // Tell GLUT to call "key" when a key is pressed
  glutKeyboardFunc(key);
  // Run main animation loop
  glutIdleFunc(display);
  glutMainLoop();
  // Return code
  return 0;
}
