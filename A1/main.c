#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// to be in arguments
int gridSize = 50;
int nIterations = 100;

int **currentgrid;
int **futuregrid;

int getNeighbours(int row, int col){
    int count = 0;

    for (int i = row-1 ; i <= row+1 ; i++){
        for (int j = col-1 ; j <= col+1 ; j++){
            if (i < 0 || j < 0 || i >= gridSize || j >= gridSize) // stay inside board
            {
                // do nothing this is outside the board
            }
            else if (currentgrid[i][j] == 1)
            {
                count++;
            }
        }
    }

    if (currentgrid[row][col] == 1)
        count = count - 1; // because it counts itself as a neighbour
    return count;
}

int main(){
    int numNeighbours = 0;

    //allocate grids
    currentgrid = calloc(gridSize, sizeof(int *));
    for (int i = 0 ; i<gridSize ; i++){
        currentgrid[i] = calloc(gridSize, sizeof(int));
    }
    futuregrid = calloc(gridSize, sizeof(int *));
    for (int i = 0 ; i<gridSize ; i++){
        futuregrid[i] = calloc(gridSize, sizeof(int));
    }

    // randomly initialize grid to start
    srand(time(NULL));
    for(int i = 0 ; i < gridSize ; i++){
        for (int j = 0 ; j < gridSize ; j++){
            currentgrid[i][j] = rand() % 2;
        }
    }
    //print the board
    for(int i = 0 ; i < gridSize ; i++){
        for (int j = 0 ; j < gridSize ; j++){
            printf("%d ", currentgrid[i][j]);
        }
        printf("\n");
    }
    //printf("\n--------------------\n");

    // do the iterations
    for (int k = 0 ; k < nIterations ; k++){

        // find the next (future) grids
        for(int i = 0 ; i < gridSize ; i++){
            for (int j = 0 ; j < gridSize ; j++){
                numNeighbours = getNeighbours(i, j);

                if (currentgrid[i][j] == 0 && numNeighbours == 3){ // a dead cell to be reborn
                    futuregrid[i][j] = 1;
                }
                else if (currentgrid[i][j] == 0 && numNeighbours != 3){ // stay dead
                    futuregrid[i][j] = 0;
                }
                else{ // handle the live cells
                    if (numNeighbours < 2){ // die by underpopulation
                        futuregrid[i][j] = 0;
                    }
                    else if(numNeighbours == 2 || numNeighbours == 3){ //live on
                        futuregrid[i][j] = 1;
                    }
                    else if(numNeighbours > 3){ // die by overpopulation
                        futuregrid[i][j] = 0;
                    }
                }
            }
        }

        //set current grid to next values
        for(int i = 0 ; i < gridSize ; i++){
            for (int j = 0 ; j < gridSize ; j++){
                currentgrid[i][j] = futuregrid[i][j];
            }
        }
        usleep(500000);
        system("clear");


        //print the board
        for(int i = 0 ; i < gridSize ; i++){
            for (int j = 0 ; j < gridSize ; j++){
                if(currentgrid[i][j] == 1)
                    printf("O ");
                else
                    printf(". ");
            }
            printf("\n");
        }
    //    printf("\n--------------------\n");
    }


    // FREEDOM FOR THE GRIDS
    for (int i = 0 ; i<gridSize ; i++){
        free(currentgrid[i]);
    }
    free(currentgrid);
    for (int i = 0 ; i<gridSize ; i++){
        free(futuregrid[i]);
    }
    free(futuregrid);

    return 0;
}
