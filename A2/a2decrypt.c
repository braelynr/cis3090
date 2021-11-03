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
  //char decryptionDictionary[strlen(inputString)];

  memset(letterList, '\0', strlen(inputString)); // initialize input dictionary
  memset(decryptedString, '\0', strlen(inputString)); // initialize input dictionary

  for(int i = 0 ; i < strlen(inputString) ; i++){

      if(inputString[i] != ' ' && strchr(letterList, inputString[i]) == NULL){ // if the character is not already in the input dictionary
          strncat(letterList, &inputString[i], 1);
      }
  }

  /* to test decrypt function
  printf("inputString = %s\n", inputString);
  printf("letterList = %s\n", letterList);
  strcpy(decryptionDictionary, "theca");
  printf("decryptionDictionary = %s\n", decryptionDictionary);
  decrypt(decryptionDictionary, letterList, inputString, decryptedString);
  printf("decryptedString = %s\n", decryptedString);
*/
  permuteString(letterList + 1, 0, strlen(letterList + 1) - 1, 'x');
  // the x will need to be replaced with the first letter of each string... and letter list will need to be rearanged for each thread


  free(inputString);

  return 0;
}
