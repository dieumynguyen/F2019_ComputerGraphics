/*
 *  Set projection
 */
#include "CSCIx229.h"

// void Project(double fov,double asp,double dim)
// {
//    //  Tell OpenGL we want to manipulate the projection matrix
//    glMatrixMode(GL_PROJECTION);
//    //  Undo previous transformations
//    glLoadIdentity();
//    //  Perspective transformation
//    if (fov)
//       gluPerspective(fov,asp,dim/16,16*dim);
//    //  Orthogonal transformation
//    else
//       glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
//    //  Switch to manipulating the model matrix
//    glMatrixMode(GL_MODELVIEW);
//    //  Undo previous transformations
//    glLoadIdentity();
// }

void Project(int projectionMode, int fov, double asp, double dim)
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
