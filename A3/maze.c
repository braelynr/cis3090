#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

typedef struct cell{
  int x;
  int y;
} Cell;

// LINKED LIST FUNCTIONS FROM CIS2750 //

/**
 * Node of a linked list. This list is doubly linked, meaning that it has points to both the node immediately in front
 * of it, as well as the node immediately behind it.
 **/
typedef struct listNode{
    Cell* cell;
    struct listNode* previous;
    struct listNode* next;
} Node;

/**
 * Metadata head of the list.
 * Contains no actual data but contains
 * information about the list (head and tail) as well as the function pointers
 * for working with the abstracted list data.
 **/
typedef struct listHead{
    Node* head;
    Node* tail;
} List;

/** Function to initialize the list metadata head to the appropriate function pointers. Allocates memory to the struct.
*@return pointer to the list head
**/
List * initializeList(){

  List * tmpList = malloc(sizeof(List));

	tmpList->head = NULL;
	tmpList->tail = NULL;

	return tmpList;
}

/** Clears the list: frees the contents of the list - Node structs and data stored in them -
 * without deleting the List struct
 * uses the supplied function pointer to release allocated memory for the data
 * @pre 'List' type must exist and be used in order to keep track of the linked list.
 * @post List struct still exists, list head = list tail = NULL, list length = 0
 * @param list pointer to the List-type dummy node
 * @return  on success: NULL, on failure: head of list
**/
void clearList(List* list){
    if (list == NULL){
		return;
	}

	if (list->head == NULL && list->tail == NULL){
		return;
	}

	Node* tmp;

	while (list->head != NULL){
		free(list->head->cell);
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}

	list->head = NULL;
	list->tail = NULL;
}

/** Deletes the entire linked list, freeing all memory.
* uses the supplied function pointer to release allocated memory for the data
*@pre 'List' type must exist and be used in order to keep track of the linked list.
*@param list pointer to the List-type dummy node
*@return  on success: NULL, on failure: head of list
**/
void freeList(List* list){

  clearList(list);
	free(list);
}

/**Function for creating a node for the linked list.
* This node contains abstracted (void *) data as well as previous and next
* pointers to connect to other nodes in the list
* @pre data should be of same size of void pointer on the users machine to avoid size conflicts. data must be valid.
* data must be cast to void pointer before being added.
* @post data is valid to be added to a linked list
* @return On success returns a node that can be added to a linked list. On failure, returns NULL.
* @param data - is a void * pointer to any data type.  Data must be allocated on the heap.
**/
Node* initializeNode(Cell* data){
	Node* tmpNode = (Node*)malloc(sizeof(Node));

	if (tmpNode == NULL){
		return NULL;
	}

	tmpNode->cell = data;
	tmpNode->previous = NULL;
	tmpNode->next = NULL;

	return tmpNode;
}

/**Inserts a Node at the front of a linked list.  List metadata is updated
* so that head and tail pointers are correct.
*@pre 'List' type must exist and be used in order to keep track of the linked list.
*@param list pointer to the dummy head of the list
*@param toBeAdded a pointer to data that is to be added to the linked list
**/
void insertFront(List* list, Cell* toBeAdded){
	if (list == NULL || toBeAdded == NULL){
		return;
	}

	Node* newNode = initializeNode(toBeAdded);

    if (list->head == NULL && list->tail == NULL){
        list->head = newNode;
        list->tail = list->head;
    }
    else{
		    newNode->next = list->head;
        list->head->previous = newNode;
    	  list->head = newNode;
    }
}

/**Returns a pointer to the data at the front of the list. Does not alter list structure.
 *@pre The list exists and has memory allocated to it
 *@param the list struct
 *@return pointer to the data located at the head of the list
 **/
Cell* popOffFront(List * list){
	if (list->head == NULL){
		return NULL;
	}

  Node* tmp;
  Cell *data;

  tmp = list->head;
  list->head = list->head->next;
  data = tmp->cell;
  free(tmp);

	return data;
}

///////////////////// END OF LINKED LIST FUNCTIONS //////////////////

// visit neighbours in a random order
void visitNeightbours(char **maze, Cell *current, List *toVisit, int size){
  int visitOrder[] = {1, 2, 3, 4}; // 1 = Up, 2 = Right, 3 = Down, 4 = Left
  // randomize order
  for(int i = 0 ; i < 4 ; i++){
      int randNum = rand() % 4;
      int temp = visitOrder[i];
      visitOrder[i] = visitOrder[randNum];
      visitOrder[randNum] = temp;
  }

  // visit each one
  for(int i = 0 ; i < 4; i++){
    // critical i  think
    if(visitOrder[i] == 1){ // up
      if(current->y - 2 > 0){ // check if in bounds
        if(maze[current->x][current->y - 2] == '.'){
          maze[current->x][current->y - 2] = '0'; // use thread num
          maze[current->x][current->y - 1] = '0';
          Cell *neighbour = malloc(sizeof(Cell));
          neighbour->x = current->x;
          neighbour->y = current->y - 2;
          insertFront(toVisit, (void *)(neighbour));
        }
      }
    }
    else if(visitOrder[i] == 2){ // right
      if(current->x + 2 < size){ // check if in bounds
        if(maze[current->x + 2][current->y] == '.'){
          maze[current->x + 2][current->y] = '0'; // use thread num
          maze[current->x + 1][current->y] = '0';
          Cell *neighbour = malloc(sizeof(Cell));
          neighbour->x = current->x + 2;
          neighbour->y = current->y;
          insertFront(toVisit, (void *)(neighbour));
        }
      }
    }
    else if(visitOrder[i] == 3){ // down
      if(current->y + 2 < size){ // check if in bounds
        if(maze[current->x ][current->y + 2] == '.'){
          maze[current->x][current->y + 2] = '0'; // use thread num
          maze[current->x][current->y + 1] = '0';
          Cell *neighbour = malloc(sizeof(Cell));
          neighbour->x = current->x;
          neighbour->y = current->y + 2;
          insertFront(toVisit, (void *)(neighbour));
        }
      }
    }
    else{ // left
      if(current->x - 2 > 0){ // check if in bounds
        if(maze[current->x - 2][current->y] == '.'){
          maze[current->x - 2][current->y] = '0'; // use thread num
          maze[current->x - 1][current->y] = '0';
          Cell *neighbour = malloc(sizeof(Cell));
          neighbour->x = current->x - 2;
          neighbour->y = current->y;
          insertFront(toVisit, (void *)(neighbour));
        }
      }
    }
  }
}

void generateMaze(Cell *start, char **maze, int size){
  List *toVisit = initializeList();
  Cell *visiting = start;
  do{
    visitNeightbours(maze, visiting, toVisit, size);
    free(visiting);
    visiting = popOffFront(toVisit);
  }while(visiting != NULL);

  freeList(toVisit);
}

void printMaze(char **maze, int size){
  for(int i = 0 ; i < size ; i++){
    for (int j = 0 ; j < size ; j++){
      printf("%c ", maze[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char **argv){
  int size = 0;

  if(argc < 3 || argc == 4 || argc > 5){
    printf("Usage: %s -n size [-s seed]\n", argv[0]);
    return -1;
  }

  if(strcmp(argv[0], "maze") == 0){ // serial program

  }
  else{ // parallel program

  }

  if (argc == 3){ // no seed specified
    srand(0);
    size = atoi(argv[2]);
  }
  else if(strcmp(argv[1], "-s") == 0){ // first optional arguement is seed
    srand(atoi(argv[2]));
    size = atoi(argv[4]);
  }
  else if(strcmp(argv[3], "-s") == 0){ // second optional arguement is seed
    srand(atoi(argv[4]));
    size = atoi(argv[2]);
  }

  if(size < 3){
    printf("Error: size must be an integer greater than or equal to 3\n");
    printf("Usage: %s -n size [-s seed]\n", argv[0]);
    return -1;
  }

  char **maze;
  maze = calloc(size, sizeof(char *));
  for (int i = 0 ; i < size ; i++){
      maze[i] = calloc(size, sizeof(char));
  }

  //initialize maze;
  for (int i = 0 ; i < size ; i++){
    for (int j = 0 ; j < size ; j++){
      maze[i][j] = '.';
    }
  }

  Cell *starting = malloc(sizeof(Cell));
  starting->x = 1;
  starting->y = 1;
  maze[1][1] = '0'; //use thread count

  generateMaze(starting, maze, size);
  printMaze(maze, size);

  for (int i = 0 ; i < size ; i++){
      free(maze[i]);
  }
  free(maze);

  return 0;
}
