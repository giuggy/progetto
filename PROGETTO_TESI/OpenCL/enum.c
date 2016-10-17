#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
 
#include "sort.h"
#include "enum.h"
#include "md5.h"
#include "device.h"

typedef void (*callback)(uint8_t* msg, size_t len, uint8_t* digest); //funzione di callback md5



size_t analyze(uint8_t* alphabet, char** hashes, size_t numHashes, size_t stringLength, void (*callback)(uint8_t* msg, size_t len, uint8_t* digest)) {	  
    
	long charsetLength = strlen((const char*)alphabet);  

	uint8_t* buffer = malloc(stringLength*sizeof(uint8_t)+1);  
	uint8_t hash[16];
	char str_hash[33]; // ogni byte rappresenta due caratteri esadecimali
	str_hash[32]='\0';
	  
	uint64_t iterations = 1; // numero di possibili combinazioni
	long i=0;
	for (; i<stringLength; ++i) {
		buffer[i] = 0;
		iterations *= charsetLength;		
	}
	buffer[stringLength]='\0';
	  
	uint64_t count = 0;
	size_t numHashesLeft = numHashes;
	
	 
	// itera su tutte le possibili combinazioni

	for (count=0; count<iterations && numHashesLeft > 0 ; ++count) {    
		

		 // genera permutazione in buffer
		
		chiamaEncode(buffer, alphabet, charsetLength, stringLength, count);

		callback(buffer, stringLength, hash); // salva digest MD5 in hash
		for (i = 0; i < 16; i++)
		  sprintf(&(str_hash[2*i]), "%2.2x", hash[i]); // hash convertito in una stringa    
		
		if (find(hashes, str_hash, numHashesLeft, stringLength) < numHashesLeft) {
		  --numHashesLeft; // assumo che non ci siano duplicati in hashes
		}
	  }
	  
	  free(buffer);
		
	  return numHashesLeft; // restituisce il numero di password ancora non decifrate


}
