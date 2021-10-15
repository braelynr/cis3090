#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// to be in arguments
int gridSize = 20;
int nIterations = 500;
int nThreads = 1;

int **currentgrid;
int **futuregrid;
int x, y;
pthread_mutex_t index_mutex;

int getNeighbours(int row, int col){
    int count = 0;
    //printf("(%d, %d)\n", row, col);

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
    if(currentgrid[row][col] == 1){
        count = count - 1; // because it counts itself as a neighbour
    }
    return count;
}

void updateCell(int i, int j){
    int numNeighbours = getNeighbours(i, j);

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

void* updateGrid()
{
    while (x < gridSize){ // rows
        while (y < gridSize){ // columns
            pthread_mutex_lock(&index_mutex);
            y++;
            pthread_mutex_unlock(&index_mutex);
            updateCell(x, y-1);
        }
        pthread_mutex_lock(&index_mutex);
        x++;
        y = 0;
        pthread_mutex_unlock(&index_mutex);
    }

    return NULL;
}

int main(int arg, char **args){

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
    printf("\n--------------------\n");

    // declare the threads
    pthread_t *threads = calloc(nThreads, sizeof(pthread_t));

    // do the iterations
    for (int k = 0 ; k < nIterations ; k++){
        x = 0;
        y = 0;

        // create threads
        for(int i = 0 ; i < nThreads ; i++){
            int test = pthread_create(&threads[i], NULL, updateGrid, NULL);
        }

        // join threads
        for (int i = 0 ; i < nThreads ; i++){
            pthread_join(threads[i], NULL);
        }

        //set current grid to next values
        for(int i = 0 ; i < gridSize ; i++){
            for (int j = 0 ; j < gridSize ; j++){
                currentgrid[i][j] = futuregrid[i][j];
            }
        }

        //print the board
        for(int i = 0 ; i < gridSize ; i++){
            for (int j = 0 ; j < gridSize ; j++){
                if(currentgrid[i][j] == 1)
                    printf("%d ", currentgrid[i][j]);
                else
                    printf(". ");
            }
            printf("\n");
        }
        //printf("\n--------------------\n");
        sleep(1);
        system("clear");

    }

    free(threads);

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
