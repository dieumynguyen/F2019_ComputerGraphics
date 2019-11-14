#include <stdio.h>
#include <stdlib.h>

int main() {
    int i, j;
    double concentrationMap[301][301];
    FILE *mapFile;
    mapFile = fopen("array.txt", "r");

    for (i=0; i<301; i++) {
        for (j=0; j<301; j++) {
            fscanf(mapFile, "%lf", &concentrationMap[i][j]);
            // printf("%f\n", concentrationMap[i][j]);
        }
    }

    printf("%f\n", concentrationMap[100][100]);

    fclose(mapFile);
    return 0;
}
