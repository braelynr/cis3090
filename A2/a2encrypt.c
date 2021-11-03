#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

char *shuffleDictionary(char *inputDictionary){
    char *encryptionDictionary = calloc(strlen(inputDictionary), sizeof(char));
    int randNum = 0;

    strcpy(encryptionDictionary, inputDictionary);

    srand(time(NULL));

    for(int i = 0 ; i < strlen(inputDictionary) ; i++){ // move each character in the string to a random position
        randNum = rand() % (strlen(inputDictionary));
        //printf("%d\n", randNum); // the position to swap with     // do i need to account for things that dont get swapped?
        char temp = encryptionDictionary[i];
        encryptionDictionary[i] = encryptionDictionary[randNum];
        encryptionDictionary[randNum] = temp;
    }
    return encryptionDictionary;
}

void encryptString(char *inputString, char *inputDictionary, char *encryptionDictionary){
    char *c;

    for(int i = 0 ; i < strlen(inputString) ; i++){
        if(inputString[i] != ' '){ // we just want to ignore the spaces
            c = strchr(inputDictionary, inputString[i]); // get a pointer to the current character in the encryptionDictionary
            inputString[i] = encryptionDictionary[(int)(c - inputDictionary)]; // c - inputDictionary finds the index of the character
        }
    }
}

void writeToFile(char *inputString){
    FILE *fp;

    fp = fopen("ciphertext.txt", "w+");
    fputs(inputString, fp);
    fclose(fp);
}

int main(int argc, char **argv){

    if(argc != 2){ // too arguments
        printf("Usage: a2encrypt inputString\n");
        return -1;
    }

    char *inputString = argv[1];
    char inputDictionary[strlen(inputString)]; // strlen(inputString) is the max possible length it could be

    memset(inputDictionary, '\0', strlen(inputString)); // initialize input dictionary

    for(int i = 0 ; i < strlen(inputString) ; i++){

        if(inputString[i] != ' ' && strchr(inputDictionary, inputString[i]) == NULL){ // if the character is not already in the input dictionary
            strncat(inputDictionary, &inputString[i], 1);
        }
    }

    // create the encryption dictionary
    char *encryptionDictionary = shuffleDictionary(inputDictionary);

    //printf("encryptionDictionary = %s\n", encryptionDictionary);

    // encrypt the string
    encryptString(inputString, inputDictionary, encryptionDictionary);

    // write to ciphertext.txt
    writeToFile(inputString);

    free(encryptionDictionary);

    return 0;
}
