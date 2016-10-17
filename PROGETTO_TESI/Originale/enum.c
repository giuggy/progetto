#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "md5.h"
#include "sort.h"

#if VERBOSE // flag passato a tempo di compilazione (-DVERBOSE)
  #include <sys/time.h>
  #define PERF_WINDOW 2*1E+06 // mostra statistiche ogni PERF_WINDOW combinazioni analizzate

  double get_real_time() {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      return tv.tv_sec+tv.tv_usec*1E-06;
  }
#endif

// genera la permutazione counter-esima dati un alfabeto ed una base
inline void encode(uint8_t* alphabet, size_t charsetLength, size_t stringLength, uint64_t counter, /*uint8_t* base,*/ uint8_t* buffer ) {
  long j=0, a=0, carry=0;
  for ( ; j < stringLength; ++j, counter/= charsetLength) {    
    a = /*base[j] +*/ carry + (counter % charsetLength); // aggiorna cella j-esima    
    carry = a / charsetLength; // tiene conto del riporto per lo step successivo
    a -= carry * charsetLength; // ... ma anche per quello corrente!       
    buffer[j] = alphabet[a]; // costruisce la stringa
  }
}
    
// analizza tutte le combinazioni di una data lunghezza  
size_t analyzeLength(uint8_t* alphabet, char* hashes[], size_t numHashes, size_t stringLength, void (*callback)(uint8_t* msg, size_t len, uint8_t* digest)) {  
  long charsetLength = strlen((const char*)alphabet);  
  //uint8_t* base = calloc(stringLength*sizeof(uint8_t), 1);
  uint8_t* buffer = malloc(stringLength*sizeof(uint8_t)+1);  
  uint8_t hash[16];
  char str_hash[33]; // ogni byte rappresenta due caratteri esadecimali
  str_hash[32]='\0';
  
  uint64_t iterations = 1; // numero di possibili combinazioni
  long i=0;
  for (; i<stringLength; ++i) {
    //base[i] = 0;
    buffer[i] = 0;
    iterations *= charsetLength;
  }
  buffer[stringLength]='\0';
  
  
  uint64_t count = 0;
  size_t numHashesLeft = numHashes;
  
  #if VERBOSE
    double start, end;  
    uint32_t window = PERF_WINDOW;    
    fprintf(stderr, "[info] analyzing passwords of length %d...\n", (int)stringLength);
    start=get_real_time();  
  #endif
  
  // itera su tutte le possibili combinazioni
//creo tanti threads quante sono le iterazioni -> il work è un metodo che richiama encode + MD5 + finde -> come arg gli devo passare il suo numero count -> il metodo encode deve essere fatto in una sezione critica ed ogni thread deve avere un proprio buffer dove memorizzare la password a lui assegnata -> deve essere protetta anche il buffer che contiene tutte le password da provare 
  for (count=0; count<iterations && numHashesLeft > 0 ; ++count) {
    #if VERBOSE
      if (!--window) {
        end = get_real_time();      
        fprintf(stderr, "[info] analyzed passwords per second: %.2f (length=%d)\n", ((double)PERF_WINDOW)/(end-start), (int)stringLength);
        window = PERF_WINDOW;
        start = get_real_time();      
      }
    #endif    
    
    encode(alphabet, charsetLength, stringLength, count, /*base,*/ buffer); // genera permutazione in buffer
    
    callback(buffer, stringLength, hash); // salva digest MD5 in hash
    for (i = 0; i < 16; i++)
      sprintf(&(str_hash[2*i]), "%2.2x", hash[i]); // hash convertito in una stringa    
    
    if (findAndRemove(hashes, str_hash, numHashesLeft) < numHashesLeft) {
      --numHashesLeft; // assumo che non ci siano duplicati in hashes
      printf("%s %s\n", str_hash, buffer);
    }
  }
  
  //free(base);
  free(buffer);
    
  return numHashesLeft; // restituisce il numero di password ancora non decifrate
}
