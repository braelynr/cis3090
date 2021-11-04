#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void readFile(char *inputString){
  FILE *fp;

  fp = fopen("ciphertext.txt", "r");
  fgets(inputString, 30, fp);
  fclose(fp);
}

void decrypt(char *decryptionDictionary, char *letterList, char *inputString, char *decryptedString){
  char *c;

  for (int i = 0 ; i < strlen(inputString) ; i++){
    if(inputString[i] == ' '){
      decryptedString[i] = ' ';
    }
    else{
      c = strchr(letterList, inputString[i]); // get a pointer to the current character in the letterList
      decryptedString[i] = decryptionDictionary[(int)(c - letterList)]; // c - letterList finds the index of the character
    }
  }
}

void swap(char *a, char *b){
  char temp = *a;
  *a = *b;
  *b = temp;
}

// permuteString function modified from https://www.geeksforgeeks.org/write-a-c-program-to-print-all-permutations-of-a-given-string/
void permuteString(char *string, int index, int length, char firstLetter){
  int i;
  if (index == length)
      printf("%c%s\n", firstLetter, string); // i think i want to skip the first character for permutating?
  else
  {
      for (i = index; i <= length; i++)
      {
          swap((string+index), (string+i));
          permuteString(string, index+1, length, firstLetter);
          swap((string+index), (string+i)); //backtrack
      }
  }
}

int main(int argc, char **argv){
  char *inputString = calloc(30, sizeof(char));

  readFile(inputString);

  char letterList[strlen(inputString)]; // strlen(inputString) is the max possible length it could be
  char decryptedString[strlen(inputString)];
  char decryptionDictionary[strlen(inputString)];  /* String storing decryptionDictionary of each process */

  memset(letterList, '\0', strlen(inputString)); // initialize input dictionary
  memset(decryptedString, '\0', strlen(inputString)); // initialize input dictionary

  for(int i = 0 ; i < strlen(inputString) ; i++){

      if(inputString[i] != ' ' && strchr(letterList, inputString[i]) == NULL){ // if the character is not already in the input dictionary
          strncat(letterList, &inputString[i], 1);
      }
  }

  int comm_sz; /* Number of processes    */
  int my_rank; /* My process rank        */

  /* Start up MPI */
  MPI_Init(NULL, NULL);

  /* Get the number of processes */
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Get my rank among all the processes */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank != 0) { // other processes
    MPI_Recv(letterList, strlen(inputString), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    permuteString(letterList + 1, 0, strlen(letterList + 1) - 1, letterList[0]);
  } else { // main process
    // I need to send each starting decryptionDictionary
    for(int i = 1 ; i < comm_sz ; i++){ // assumes the number of processes is the same as the number of unique characters
      swap(&letterList[0],&letterList[i]);
      MPI_Send(letterList, strlen(letterList)+1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      swap(&letterList[0],&letterList[i]);
    }
    //this process will handle the original (non swapped) letterList permutations
    permuteString(letterList + 1, 0, strlen(letterList + 1) - 1, letterList[0]);
  }

  /* Shut down MPI */
  MPI_Finalize();



  free(inputString);

  return 0;
}
