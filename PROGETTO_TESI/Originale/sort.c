#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

int textCompare(const void *a, const void *b) {
  char *m1 = *(char **)a;
  char *m2 = *(char **)b;
  return strcmp(m1, m2);
}

void sortArrayOfString(char** strings, size_t numLines) {
  qsort(strings, numLines, sizeof(char*), textCompare);
}  

size_t findAndRemove(char** strings, char* s, size_t numLines) {
  char* ret = (char*)bsearch(&s, strings, numLines, sizeof(char*), textCompare);  
  if (ret == NULL)
    return numLines;  
  
  // scambio l'elemento con quello in ultima posizione  
  char* tmp = strdup(*(char**)ret); // analogo alla textCompare()
  memcpy(*(char**)ret, strings[numLines-1], 32);
  memcpy(strings[numLines-1], tmp, 32);
  free(tmp);  
  
  sortArrayOfString(strings, numLines-1); // riordino l'array per la ricerca binaria
  
  return numLines-1; // rimozione lazy
}    
