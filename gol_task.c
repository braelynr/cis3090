#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// to be in arguments
int gridSize = 10;
int nIterations = 10;

int **currentgrid;
int **futuregrid;
pthread_mutex_t futuregrid_mutex;
int doneWork;

//****************************** queue stuff ******************************
//The following section of code was taken and modified from the file msgqueue.c which was provided to the class

//gridcell struct
typedef struct {
    int row;
    int col;
} Cell;

//Queue node
typedef struct queue_node {
    Cell cell;
    struct queue_node* next;
} QueueNode;

//queue - a singly linked list
//Remove from head, add to tail
typedef struct {
    QueueNode* head;
    QueueNode* tail;
    pthread_mutex_t mutex;
} Queue;

//Create a queue and initilize its mutex
Queue* createQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    return q;
}

//append to the queue
void addNode(Queue* q, int row, int col)
{
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    node->cell.row = row;
    node->cell.col = col;
    node->next = NULL;

    // critical section
    //Obtain lock
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node;       // append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node;   // first node
    }
    //Release lock
    pthread_mutex_unlock(&q->mutex);
    //sleep(5); //why?
}
//pull (remove) from the queue
int removeNode(Queue* q, Cell* cell_out)
{
    int success = 0;

    // critical section
    //Obtain lock
    pthread_mutex_lock(&q->mutex);
    if (q->head != NULL) {
        QueueNode* oldHead = q->head;
        *cell_out = oldHead->cell;    // copy out the cell info
        q->head = oldHead->next;
        if (q->head == NULL) {
            q->tail = NULL;         // last node removed
        }
        free(oldHead);
        success = 1;
    }
    //Release lock
    pthread_mutex_unlock(&q->mutex);
    return success;
}
//*****************************************************************************

void deleteQueue(Queue *toDelete){
    //note this currently assumes the queues are already empty
    free(toDelete);
}

// global variable queues
Queue *livingCells;
Queue *deadCells;

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

void* populateQueues(){
    int numNeighbours = 0;

    for(int i = 0 ; i < gridSize ; i++){
        for (int j = 0 ; j < gridSize ; j++){
            numNeighbours = getNeighbours(i, j);
            if (currentgrid[i][j] == 0 && numNeighbours == 3){ // a dead cell to be reborn
                addNode(livingCells, i, j);
            }
            else if (currentgrid[i][j] == 0 && numNeighbours != 3){ // stay dead
                addNode(deadCells, i, j);
            }
            else{ // handle the live cells
                if (numNeighbours < 2){ // die by underpopulation
                    addNode(deadCells, i, j);
                }
                else if(numNeighbours == 2 || numNeighbours == 3){ //live on
                    addNode(livingCells, i, j);
                }
                else if(numNeighbours > 3){ // die by overpopulation
                    addNode(deadCells, i, j);
                }
            }
        }
    }
    doneWork = 1;
}

void* fillLiving(){
    Cell *toUpdate = (Cell*)malloc(sizeof(Cell));
    int done = 0; // 0 = still work to do
    int success = 0;

    while (done == 0){ // how to implement so queues dont leave until board is actually filled?
        success = removeNode(livingCells, toUpdate);
        if(success == 1){
            pthread_mutex_lock(&futuregrid_mutex);
            futuregrid[toUpdate->row][toUpdate->col] = 1;
            pthread_mutex_unlock(&futuregrid_mutex);
        }
        else{
            // check if we are done completely
            if (doneWork == 0){
                // we're not done wait for more to queue
            }
            else{
                done = 1; // queue is empty and will not be filled with anything else
            }

        }
    }
    free(toUpdate);
}

void* fillDead(){
    Cell *toUpdate = (Cell*)malloc(sizeof(Cell));
    int done = 0; // 0 = still work to do

    while (done == 0){ // how to implement so queues dont leave until board is actually filled?
        if(removeNode(deadCells, toUpdate) == 1){
            pthread_mutex_lock(&futuregrid_mutex);
            futuregrid[toUpdate->row][toUpdate->col] = 0;
            pthread_mutex_unlock(&futuregrid_mutex);
        }
        else{
            // check if we are done completely
            if (doneWork == 0){
                // we're not done wait for more to queue
            }
            else{
                done = 1; // queue is empty and will not be filled with anything else
            }
        }
    }
    free(toUpdate);
}

int main(int arg, char **args){

    // signals for the queues to wait on eachother
    doneWork = 0;

    livingCells = createQueue();
    deadCells = createQueue();
    pthread_mutex_init(&futuregrid_mutex, NULL);

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

    pthread_t queueFiller;
    pthread_t liveReader;
    pthread_t deadReader;

    // // do the iterations
    for (int k = 0 ; k < nIterations ; k++){

        pthread_create(&queueFiller, NULL, populateQueues, NULL);
        pthread_create(&liveReader, NULL, fillLiving, NULL);
        pthread_create(&deadReader, NULL, fillDead, NULL);

    // so we need some barriers

        pthread_join(queueFiller, NULL);

        pthread_join(liveReader, NULL);

        pthread_join(deadReader, NULL);


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
                    printf("O ");
                else
                    printf(". ");
            }
            printf("\n");
        }
        printf("\n--------------------\n");

    }

    // FREEDOM FOR THE QUEUES
    deleteQueue(livingCells);
    deleteQueue(deadCells);

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
