#include "CSCIx229.h"

/* ====================== GLOBALS ====================== */
int th = 180;           // Azimuth
int ph = 90;          // Elevation
double zh = 90;       // Rotation
int axes = 1;         // Display axes

// For projections
int firstPerson = 0;     // First person mode is perspective
int projectionMode = 1;  // Projection mode: 0 for orthographic, 1 for perspective
int fov = 55;            // Field of view for perspective
double asp = 1;          // Aspect ratio
double dim = 4.0;        // Size of world; 4
// For FP Perspective
float xPosition = 0, yPosition = 0, zPosition = 4;  // Position of camera
float xRotation = 0, yRotation = 0, zRotation = 0;  // Rotation axes
float angle = 0.0;									// Angle of rotation
float cRadius = 4.0;                   				// Our distance from the camera
float xLast, yLast; 								// Previous x,y

float platformHeight = -0.115;

// For lighting
int moveLight = 1;
int light = 1;
// Light values
int one = 1;       // Unit value
int inc = 5;       // Ball increment
int smooth = 1;    // Smooth/Flat shading
int local = 0;     // Local Viewer Model
int emission = 0;  // Emission intensity (%)
int ambient = 50;  // Ambient intensity (%)
int diffuse = 100; // Diffuse intensity (%)
int specular = 0;  // Specular intensity (%)
int shininess = 0; // Shininess (power of two)
float shiny = 1;   // Shininess (value)
float ylight = 0.5;  // Elevation of light
float distance = 2.0;  // Light distance

// Textures
int textureMode = 0;
unsigned int texture[2]; // Texture names

// Animate wings
int time_step = 0;
int num_timesteps = 90;
float wingRate = 15;


float X=0,Y=0,Z=1; //  Mandelbrot X,Y,Z


////////////////////////////
// Shader variables
int shaderMode = 0;
int shader[] = {0};
#ifdef GL_VERSION_4_0
#define MODE 11
#else
#define MODE 10
#endif
////////////////////////////


////////////////////////////
// Concentration map variables
int i, j;
int arenaSize = 301;
double concentrationMap[301][301];
FILE *mapFile;
////////////////////////////

int numBees = 25;
double positionArray[25][3];
FILE *posFile;

/* ====================== DISPLAY ====================== */
// OpenGL (GLUT) calls this routine to display the scene
void display()
{
    const double len = 1.5;  // Length of axes
    // Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    // Undo previous transformations
    glLoadIdentity();

    /* ========= PERSPECTIVE STUFF ========= */
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
        ball(Position[0], Position[1], Position[2], 0.1, emission, shiny);
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


    /* ========= GET CONCENTRATION MAPS ========= */
    // Read in h5 concentration map
    if (time_step < 5000) {
        char filepath[60];
        snprintf(filepath, 60, "abm_data/concentrationMaps/timestep_%d.txt", time_step);

        // Read in external data to visualize concentration map
        mapFile = fopen(filepath, "r");
        for (i=0; i<arenaSize; i++) {
            for (j=0; j<arenaSize; j++) {
                fscanf(mapFile, "%lf", &concentrationMap[i][j]);
            }
        }
        fclose(mapFile);
    }

    /* ========= GET BEE POSITIONS ========= */
    if (time_step < 5000) {
        char filepath2[60];
        snprintf(filepath2, 60, "abm_data/positionArrays/timestep_%d.txt", time_step);

        // Read in external data to visualize concentration map
        posFile = fopen(filepath2, "r");
        for (i=0; i<numBees; i++) {         // Loop over bees
            for (j=0; j<3; j++) {           // Loop over a bee's XY
                fscanf(mapFile, "%lf", &positionArray[i][j]);
            }
        }
        // printf("X: %lf\n", positionArray[3][0]);
        // printf("Y: %lf\n", positionArray[3][1]);
        // printf("State: %lf\n", positionArray[3][2]);
        fclose(posFile);
    }


    /* ========= CALL SHAPES HERE ========= */
    // Platform for pheromone plumes
    // ground(float x,float y,float z, float scale, concentrationMap)
    ground(0,platformHeight,0, 3, concentrationMap);

    // Bee: x,y,z, azimuth,elevation,theta, scale, head_z, x_scalar,y_scalar,z_scalar, wing_angle
    // All bees should be at same y
    // Queen bee
    float wingAngle = 0;
    bee(0,0,0, 0,0,0, 0.7, 0.35, 0.3,0.3,1.5, wingAngle, textureMode, texture);

    // Draw worker bees for this time step
    int bee_i;
    double x_position, y_position;
    for (bee_i=0; bee_i<numBees; bee_i++) {
        x_position = positionArray[bee_i][0];
        y_position = positionArray[bee_i][1];

        // Flap wings if scenting
        if (positionArray[bee_i][2] == 1) {
            wingAngle = (Cos(time_step*wingRate) + 1) / 2 * 90;
        }
        else {
            wingAngle = 0;
        }

        bee(x_position,0,y_position, 0,0,0, 0.5, 0.3, 0.3,0.3,0.8, wingAngle, textureMode, texture);
    }

    time_step++;
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
    Project(projectionMode, fov, asp, dim);
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

    else if (key == 'u') {
        platformHeight += 0.04;
    }

    else if (key == 'j') {
        platformHeight -= 0.04;
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
    // Light radius
    else if (key=='{') {
        distance -= 0.1;
        if (distance <= 0) {
            distance = 0.01;
        }
    }
    else if (key=='}') {
        distance += 0.1;
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
    Project(projectionMode, fov, asp, dim);
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
    Project(projectionMode, fov, asp, dim);
}


/* ====================== SHADER FUNCTIONS ====================== */
char* ReadText(char *file)
{
  int   n;
  char* buffer;
  //  Open file
  FILE* f = fopen(file,"rt");
  if (!f) Fatal("Cannot open text file %s\n",file);
  //  Seek to end to determine size, then rewind
  fseek(f,0,SEEK_END);
  n = ftell(f);
  rewind(f);
  //  Allocate memory for the whole file
  buffer = (char*)malloc(n+1);
  if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
  //  Snarf the file
  if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
  buffer[n] = 0;
  //  Close and return
  fclose(f);
  return buffer;
}

void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}
/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER  ,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}



/* ====================== MAIN ====================== */
// Start up GLUT and tell it what to do
int main(int argc,char* argv[])
{
    // ////////////////////////////////////////////////////////
    // // Read in h5 concentration map
    // mapFile = fopen("abm_data/array.txt", "r");
    // for (i=0; i<arenaSize; i++) {
    //     for (j=0; j<arenaSize; j++) {
    //         fscanf(mapFile, "%lf", &concentrationMap[i][j]);
    //     }
    // }
    // // printf("Debug value: %f\n", concentrationMap[100][100]);
    // fclose(mapFile);
    // ////////////////////////////////////////////////////////


    // Initialize GLUT and process user parameters
    glutInit(&argc,argv);
    // Request double buffered, true color window with Z buffering at 600x600
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    // Create the window
    glutCreateWindow("Dieu My Nguyen - Final Project - Visualize pheromone network model in honey bees");
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

    // Load textures
    texture[0] = LoadTexBMP("wing.bmp");
    texture[1] = LoadTexBMP("fuzzy.bmp");

    // Shader stuff
    shader[0] = CreateShaderProg("simple.vert", "simple.frag");

    glutMainLoop();
    return 0;
}
