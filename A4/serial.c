#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void printGrid(char **grid, int size){
    for(int i = 0 ; i < size ; i++){
        for(int j = 0 ; j < size ; j++){
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

int getNeighbours(char **grid, int size, int i, int j){
    int neighbours = 0;

    for(int k = j-2 ; k <= j+2 ; k++){
        if(k == j){} // don't count itself
        else if (k >= 0 && k < size){ // stay inside grid
            if(grid[i][k] == 'X'){
                neighbours++;
            }
        }
    }

    return neighbours;
}

void runProgram(char **grid, int size){
    int neighbours;
    for(int i = 1 ; i < size ; i++){ // for each remaining row
        for(int j = 0 ; j < size ; j++){ // for each cell in the row
            neighbours = getNeighbours(grid, size, i-1, j);
            if(grid[i-1][j] == ' '){ // dead cell
                if(neighbours == 2 || neighbours == 3){
                    grid[i][j] = 'X';
                }
            }
            else{ // living cell
                if(neighbours == 2 || neighbours == 4){
                    grid[i][j] = 'X';
                }
            }
        }
    }
}

// ./a4 -s 20 -i 0 will run the program  using an array that is 20x20 units,
// and  using  the  initial  row  configuration  zero,  which  means  the  first  row  will  be random.
int main(int argc, char **argv){

    int size = 1;
    int pattern = 0;

    // handles command line arguments. Does not account for invalid arguments
    for (int i = 0 ; i < argc ; i++){
        if(strcmp(argv[i], "-s") == 0){
            size = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-i") == 0){
            pattern = atoi(argv[i+1]);
        }
    }

    char **grid;
    grid = calloc(size, sizeof(char *));
    for (int i = 0 ; i < size ; i++){
        grid[i] = calloc(size, sizeof(char));
    }

    // initialize grid
    for(int i = 0 ; i < size ; i++){
        for(int j = 0 ; j < size ; j++){
            grid[i][j] = ' ';
        }
    }

    srand(time(NULL));

    int mid = (int)(size / 2);

    switch(pattern){
        case 0: // random
            for(int i = 0 ; i < size ; i++){
                if(rand() % 2 == 0){
                    grid[0][i] = 'X';
                }
            }
            break;
        case 1: // flipflop
            grid[0][mid-1] = 'X';
            grid[0][mid+1] = 'X';
            grid[0][mid+2] = 'X';
            break;
        case 2: // spider
            grid[0][mid-2] = 'X';
            grid[0][mid-1] = 'X';
            grid[0][mid] = 'X';
            grid[0][mid+1] = 'X';
            grid[0][mid+2] = 'X';
            grid[0][mid+3] = 'X';
            break;
        case 3: // glider
            grid[0][mid-2] = 'X';
            grid[0][mid] = 'X';
            grid[0][mid+1] = 'X';
            grid[0][mid+2] = 'X';
            break;
        case 4: // face
            grid[0][mid-3] = 'X';
            grid[0][mid-2] = 'X';
            grid[0][mid-1] = 'X';
            grid[0][mid] = 'X';
            grid[0][mid+1] = 'X';
            grid[0][mid+2] = 'X';
            grid[0][mid+3] = 'X';
            break;
        default:
            printf("Invalid Pattern");
            return -1;
            break;
    }

    runProgram(grid, size);

    printGrid(grid, size);

    for (int i = 0 ; i < size ; i++){
        free(grid[i]);
    }
    free(grid);

    return 0;
}
