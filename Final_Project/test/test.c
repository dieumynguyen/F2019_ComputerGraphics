#include <stdio.h>
#include <stdlib.h>

int main() {
    int arenaSize = 301;
    int i, j;
    double concentrationMap[301][301];
    FILE *mapFile;

    char filepath[60];
    snprintf(filepath, 60, "abm_data/concentrationMaps/timestep_%d.txt", 0);

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
if (time_step == 5000) {
    char filepath2[60];
    snprintf(filepath2, 60, "abm_data/positionArrays/timestep_%d.txt", time_step);

    // Read in external data to visualize concentration map
    posFile = fopen(filepath2, "r");
    for (i=0; i<numBees; i++) {         // Loop over bees
        for (j=0; j<3; j++) {           // Loop over a bee's XY and state
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

    // // Flap wings if bee is scenting
    // if (positionArray[bee_i][2] == 1) {
    //     wingAngle = (Cos(time_step*wingRate) + 1) / 2 * 90;
    // }

    bee(x_position,0,y_position, 0,0,0, 0.5, 0.3, 0.3,0.3,0.8, wingAngle, textureMode, texture);
}
