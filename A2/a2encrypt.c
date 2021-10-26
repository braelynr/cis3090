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

int main(int argc, char **argv){

    if(argc < 2){ // too arguments
        printf("Usage: a2encrypt inputString\n");
        return -1;
    }

    // printf("argc = %d\n", argc);
    //
    // for(int i = 0 ; i < argc ; i++){
    //     printf("%s ", argv[i]);
    // }
    //
    // printf("\n\n");

    char *inputString = argv[1]; // this assumes command line is as follows: ./a2encrypt "the cat"
    char inputDictionary[strlen(inputString)]; // strlen(inputString) is the max possible length it could be

    memset(inputDictionary, '\0', strlen(inputString)); // initialize input dictionary

    for(int i = 0 ; i < strlen(inputString) ; i++){

        if(inputString[i] != ' ' && strchr(inputDictionary, inputString[i]) == NULL){ // if the character is not already in the input dictionary
            strncat(inputDictionary, &inputString[i], 1);
        }
    }

    printf("inputString = %s\n", inputString);
    printf("inputDictionary = %s\n", inputDictionary);

    char *encryptionDictionary = shuffleDictionary(inputDictionary);

    printf("encryptionDictionary = %s\n", encryptionDictionary);

    free(encryptionDictionary);

    return 0;
}
