#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv){
  char *inputString = calloc(30, sizeof(char));

  readFile(inputString);

  char letterList[strlen(inputString)]; // strlen(inputString) is the max possible length it could be
  char decryptedString[strlen(inputString)];
  char decryptionDictionary[strlen(inputString)];

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


  free(inputString);

  return 0;
}
