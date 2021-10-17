#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

// to be in arguments
int gridSize;
int nIterations;
int nThreads;

int **currentgrid;
int **futuregrid;
int x, y;
pthread_mutex_t mutex;

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
    //printf("(%d,%d)\n", i, j);

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

bool getWork(int *i, int *j){
    if (x < gridSize){
        if (y < gridSize){
            *i = x;
            *j = y;

            y++;
            return false;
        }
        else if (y >= gridSize && x < gridSize - 1){
            y = 0;
            x++;
            *i = x;
            *j = y;
            y++;
            return false;
        }
        else{
            return true;
        }
    }
    else{
        return true;
    }
}


void* updateGrid()
{
    int currentX = 0;
    int currentY = 0;
    bool end = false;

    while(end == false){
        pthread_mutex_lock(&mutex);
        end = getWork(&currentX, &currentY);
        pthread_mutex_unlock(&mutex);
        if (end == false)
            updateCell(currentX, currentY);
    }

    return NULL;
}

void printGrid(){
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
    printf("\n--------------------\n");
}

int main(int argc, char **argv){
    int print = 0; // assume false unless specified in commandline
    gridSize = 0;
    nIterations = 0;
    nThreads = 0;

    if(argc < 4 || argc > 5){ // too few or too many arguments
        printf("Usage: gol_data nThreads gridSize nInterations [-d]\n");
        return -1;
    }
    else{
        nThreads = atoi(argv[1]);
        gridSize = atoi(argv[2]);
        nIterations = atoi(argv[3]);
        if(argc == 5){
            print = 1;
        }
        if(nThreads == 0)
        {
            printf("Invalid Arguments: nThreads must be an integer greater than or equal to 1\n");
            printf("Usage: gol_data nThreads gridSize nInterations [-d]\n");
            return -1;
        }
        else if(gridSize == 0){
            printf("Invalid Arguments: gridSize must be an integer greater than or equal to 1\n");
            printf("Usage: gol_data nThreads gridSize nInterations [-d]\n");
            return -1;
        }
        else if(nIterations == 0){
            printf("Invalid Arguments: nIterations must be an integer greater than or equal to 1\n");
            printf("Usage: gol_data nThreads gridSize nInterations [-d]\n");
            return -1;
        }
    }

    pthread_mutex_init(&mutex, NULL);

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
    if(print == 1){
        printGrid();
    }

    // declare the threads
    pthread_t *threads = calloc(nThreads, sizeof(pthread_t));

    // do the iterations
    for (int k = 0 ; k < nIterations ; k++){
        x = 0;
        y = 0;

        // create threads
        for(int i = 0 ; i < nThreads ; i++){
            pthread_create(&threads[i], NULL, updateGrid, NULL);
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
        if(print == 1){
            printGrid();
        }

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
