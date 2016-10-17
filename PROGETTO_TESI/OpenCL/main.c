#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enum.h"
#include "md5.h"
#include "sort.h"

#define MAX_PWD_LENGTH      10
#define FILE_BUF_LENGTH     256

int main(int argc, char* argv[]) {
  uint8_t* alphabet=(uint8_t*)"0123456789abcdefghijklmnopqrstuvwxyz";
  
  if (argc != 2) {
    printf("Syntax: %s <input_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  char buf[FILE_BUF_LENGTH+1];
  size_t numLines=0;
  FILE* input_file = fopen(argv[1], "r");
  if (input_file == NULL) {
    printf("Cannot open file %s!\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  
  // conta il numero di linee nel file
  while (fgets(buf, FILE_BUF_LENGTH, input_file) != NULL)
    numLines++;
  rewind(input_file);

  // costruisci un array di stringhe
  char **hashes = malloc(numLines*sizeof(char*));  
  size_t i=0; 
  
  
     
  while (fgets(buf, FILE_BUF_LENGTH, input_file) != NULL) {
    hashes[i] = malloc(33*sizeof(char));
    hashes[i][32] = '\0';   
    memcpy(hashes[i++], buf, 32);    
  }
  fclose(input_file);  

  // ordina l'array
  sortArrayOfString(hashes, numLines);
  
  
          
  // analizza permutazioni di lunghezza i crescente
  size_t ret=numLines;
  for (i=1; i<=MAX_PWD_LENGTH && ret>0; ++i)
    ret = analyze(alphabet, hashes, ret, i, md5);
    
    
    
  if (ret>0)
    printf("Uncracked passwords: %d\n", (int)ret);
  else
    printf("All passwords have been cracked!\n");   
  
  // libera la memoria allocata  
  for(i=0; i<numLines; ++i)
    free(hashes[i]);
  free(hashes);
  
  return 0;
}
