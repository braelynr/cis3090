// CIS*3090 A2
// Braelyn Rotman
// 1006740
// November 5th, 2021


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void readFile(char *inputString){
  FILE *fp;

  fp = fopen("ciphertext.txt", "r");
  fgets(inputString, 50, fp);
  fclose(fp);
}

// decrypt the string using the decryptionDictionary
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

// check the results against the dictionary
void evalutateDecryption(char *decryptedString){

    int valid = 1; // 1 = valid, 0 = invalid
    char c[2] = " "; // word delimiter
    char command[100];
    char tokenizeMe[50];
    strcpy(tokenizeMe, decryptedString); // because strtok modifies the original string

    char *token = strtok(tokenizeMe, c);

    /* walk through other tokens */
    while( token != NULL ) {
        memset(command, '\0', 100);
        strcat(command, "grep \"^");
        strcat(command, token);
        strcat(command, "$\" /usr/share/dict/words > grepOutput");
        if(system(command) != 0){ // grep returns 0 if word is found
          valid = 0;
          break;
        }
        token = strtok(NULL, c);
    }

    if(valid == 1){
        printf("%s\n", decryptedString);
    }

}

// permuteString function modified from https://www.geeksforgeeks.org/write-a-c-program-to-print-all-permutations-of-a-given-string/
void permuteString(char *string, int index, int length, char firstLetter, char *letterList, char *inputString){
    int i;
    if (index == length){
        char decryptedString[50];
        char decryptionDictionary[50];  /* String storing decryptionDictionary of each process */
        memset(decryptedString, '\0', 50);
        memset(decryptionDictionary, '\0', 50);
        strncat(decryptionDictionary, &firstLetter, 1);
        strcat(decryptionDictionary, string);

        decrypt(decryptionDictionary, letterList, inputString, decryptedString);
      //  printf("decryptionDictionary=%s\nletterList=%s\ninputString=%s\ndecryptedString=%s\n\n", decryptionDictionary, letterList, inputString, decryptedString);

        evalutateDecryption(decryptedString);
    }
    else
    {
        for (i = index; i <= length; i++)
        {
            swap((string+index), (string+i));
            permuteString(string, index+1, length, firstLetter, letterList, inputString);
            swap((string+index), (string+i)); //backtrack
        }
    }
}

int main(int argc, char **argv){

  char inputString[50];
  char originalPermutation[50];
  char letterList[50];
  int comm_sz; /* Number of processes    */
  int my_rank; /* My process rank        */

  /* Start up MPI */
  MPI_Init(NULL, NULL);

  /* Get the number of processes */
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /* Get my rank among all the processes */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank != 0) { // other processes
    MPI_Recv(inputString, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(originalPermutation, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(letterList, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    permuteString(letterList + 1, 0, strlen(letterList + 1) - 1, letterList[0], originalPermutation, inputString);
  } else { // main process
    readFile(inputString);

    memset(letterList, '\0', 50); // initialize input dictionary


    for(int i = 0 ; i < strlen(inputString) ; i++){

        if(inputString[i] != ' ' && strchr(letterList, inputString[i]) == NULL){ // if the character is not already in the input dictionary
            strncat(letterList, &inputString[i], 1);
        }
    }
    strcpy(originalPermutation, letterList);

    for(int i = 1 ; i < comm_sz ; i++){
      MPI_Send(inputString, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      MPI_Send(originalPermutation, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }

    // I need to send each starting decryptionDictionary
    for(int i = 1 ; i < comm_sz ; i++){ // assumes the number of processes is the same as the number of unique characters
      swap(&letterList[0],&letterList[i]);
      MPI_Send(letterList, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      swap(&letterList[0],&letterList[i]);
    }
    //this process will handle the original (non swapped) letterList permutations
    permuteString(letterList + 1, 0, strlen(letterList + 1) - 1, letterList[0], originalPermutation, inputString);
  }

  /* Shut down MPI */
  MPI_Finalize();



  return 0;
}
