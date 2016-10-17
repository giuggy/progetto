#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
 
#include "sort.h"


int textCompare(const void *a, const void *b) {
  char *m1 = *(char **)a;
  char *m2 = *(char **)b;
  return strcmp(m1, m2);
}

void sortArrayOfString(char** strings, size_t numLines) {
  qsort(strings, numLines, sizeof(char*), textCompare);
} 
//rimuove la password dal buffer hashes
size_t rimozione(char* ret,char** strings, size_t numLines)
{
		
		char* tmp = strdup(*(char**)ret); // analogo alla textCompare()
		memcpy(*(char**)ret, strings[numLines-1], 32);
		memcpy(strings[numLines-1], tmp, 32);
		free(tmp);  

		
		sortArrayOfString(strings, numLines-1); // riordino l'array per la ricerca binaria
		numLines--;

		return numLines; // rimozione lazy
}  

//cerca le password

size_t find(char** strings, char* s, size_t numLines){

		int ret= sem_wait(&x);
		ERROR_HELPER(ret, "Errore Wait");
		readcount+=1;
		if(readcount==1) {
			ret=sem_wait(&wsem);
			ERROR_HELPER(ret, "Errore Wait");
		}
		ret=sem_post(&x);
		ERROR_HELPER(ret, "Errore Post");
		char* et = (char*)bsearch(&s, strings, numLines, sizeof(char*), textCompare); 

		if (et != NULL){
						
			numLines= rimozione(et, strings, numLines);
			
		}
		ret=sem_wait(&x);
		ERROR_HELPER(ret, "Errore Wait");
		readcount-=1;
		if(readcount==0) {
			ret=sem_post(&wsem);
			ERROR_HELPER(ret, "Errore Post");
		}
		ret=sem_post(&x);
		ERROR_HELPER(ret, "Errore Post");
	
		
		return numLines;
}



/*size_t findAndRemove(char** strings, char* s, size_t numLines) {
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
}    */
