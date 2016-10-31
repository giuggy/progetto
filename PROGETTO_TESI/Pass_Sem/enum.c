#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <semaphore.h>
 
#include "sort.h"
#include "enum.h"


#define NUM_THREADS 4



//definizione parametri dei thread

typedef void (*callback)(uint8_t* msg, size_t len, uint8_t* digest); //funzione di callback md5

typedef struct{
	int thread_id; //id del thread
	uint64_t  da; // da dove il thread deve iniziare ad analizzare
	uint64_t  a; // indice dell'ultima cella che deve analizzare il thread
	uint8_t* buf; //buffer con testo da analizzare
	uint8_t* alpha; //buffer dell'alfabeto usato
	char** hashe; //buffer con le password
	size_t stringL; //lunghezza password analizzata 
	long charsetL; //buffer
	callback f;
	

    
} thread_find;



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
    a = /*base[j] + */carry + (counter % charsetLength); // aggiorna cella j-esima    
    carry = a / charsetLength; // tiene conto del riporto per lo step successivo
    a -= carry * charsetLength; // ... ma anche per quello corrente!       
    buffer[j] = alphabet[a]; // costruisce la stringa
    
  }
}
//inizializzazione semafori
void iniziaSem(){
	extern sem_t x;
	extern sem_t wsem;
	int ret = sem_init(&x, 0,1);
	ERROR_HELPER(ret, "Errore inizializzazione sem x");
	
	ret=sem_init(&wsem,0,1);
	ERROR_HELPER(ret, "Errore inizializzazione sem wsem");
}

//rimozione semafori
void liberaSem(){
	int ret;
	ret=sem_destroy(&x);
	ERROR_HELPER(ret, "Errore distruzione sem x");
	ret=sem_destroy(&wsem);
	ERROR_HELPER(ret, "Errore distruzione sem wsem");
}

//funzione dei thread -> richiama find
void* work(void* par){

	uint8_t hash[16];
	char str_hash[33]; // ogni byte rappresenta due caratteri esadecimali
	str_hash[32]='\0';
	uint64_t count; 
	int i; 
	
	// ridefinizione indici
	thread_find* d= (thread_find*) par;
	
	uint64_t from= d-> da;
	uint64_t to= d-> a;
	uint8_t* alphabet = d -> alpha; 
	long charsetLength = d -> charsetL;
	size_t stringLength = d -> stringL;
	
	//size_t numHashesLeft = d -> numHashesL;
	//uint8_t* base = d -> bas; 
	
	uint8_t* buffer = d -> buf;
	
	/*
	printf ("da %d \n", (int)from);
	printf ("a %d \n", (int) to);
	printf ("alfabeto %s \n", (char*)alphabet);
	printf ("lunghezza charset %ld \n", charsetLength);
	printf ("lunghezza stringa %d \n", (int) stringLength);
	*/
	
	#if VERBOSE
	double start, end;  
    uint32_t window = PERF_WINDOW;    
    fprintf(stderr, "[info] analyzing passwords of length %d...by thread %d \n", (int)stringLength, d -> thread_id);
    start=get_real_time();  
	#endif
	
	if (numH <1) pthread_exit(NULL);
	
	for (count= from; count<to && numH > 0 ; count++) {
		
				
		#if VERBOSE
		if (!--window) {
			end = get_real_time();      
			fprintf(stderr, "[info] analyzed passwords per second: %.2f\n", ((double)PERF_WINDOW)/(end-start));
			window = PERF_WINDOW;
			start = get_real_time();      
		}
		#endif    
   
		encode(alphabet, charsetLength, stringLength, count, buffer); // genera permutazione in buffer

		
		
		d -> f(buffer, stringLength, hash); // salva digest MD5 in hash
		
		
		
		for (i = 0; i < 16; i++){ 
			sprintf(&(str_hash[2*i]), "%2.2x", hash[i]); // hash convertito in una stringa   
			
		}

		
		size_t numL = numH;
		

		int ff=-10;
		if ((ff=find(d -> hashe, str_hash, numL) )< numH){
			numH-=1; // assumo che non ci siano duplicati in hashes
		}


	}
	return NULL;
}
    
// analizza tutte le combinazioni di una data lunghezza  
size_t analyzeLength(uint8_t* alphabet, char** hashes, size_t numHashes, size_t stringLength, void(*callback)(uint8_t* msg, size_t len, uint8_t* digest)) {  
  
  long charsetLength = strlen((const char*)alphabet);  

  uint8_t* buffer = malloc(stringLength*sizeof(uint8_t)+1); 
  
  
  
  
  
  //inizializzo varabili comuni prima della creazione dei thread
  extern size_t numH;
  numH = numHashes;
  extern int readcount;
  readcount=0;
  
  uint64_t iterations = 1; // numero di possibili combinazioni
  long i=0;
  for (; i<stringLength; ++i) {

    iterations *= charsetLength;  
	buffer[i] = 0;
	}
	buffer[stringLength]='\0';
	
  
  int j,res;
  iniziaSem();
  
  //creazione thread
  

  pthread_t* t = (pthread_t*) malloc(NUM_THREADS*sizeof(pthread_t));
  
  for (j=0; j< NUM_THREADS; j++){
	thread_find* d = (thread_find*) malloc(NUM_THREADS*sizeof(thread_find));
		
	d -> thread_id = j;
	
	//setto inizio e fine di ogni thread 	
			
	d -> da = j*(iterations/NUM_THREADS);
	if( j< NUM_THREADS - 1){
		d -> a = ((j+1)*(iterations/NUM_THREADS));
	} else {
		d -> a = iterations;
	}
			
	//setto altri parametri
		d -> buf =  buffer;
		d -> alpha = alphabet;
		d -> stringL = stringLength;
		d -> charsetL = charsetLength;
		d -> hashe=hashes;
		d -> f= callback;

		
		res=pthread_create(&t[j],NULL,work,(void*) d);
		assert(res==0);
	}	
  
  //chiamata join per i thread
  for( j = 0; j< NUM_THREADS; j++){
	pthread_join(t[j],NULL);
  }
  
  //liberazione risorse
  
  liberaSem();
  free(t);
  free(buffer);
  
  size_t risultato = numH;
    
  return risultato; // restituisce il numero di password ancora non decifrate
}

  /*
  #if VERBOSE
    double start, end;  
    uint32_t window = PERF_WINDOW;    
    fprintf(stderr, "[info] analyzing passwords of length %d...\n", (int)stringLength);
    start=get_real_time();  
  #endif
  
  // itera su tutte le possibili combinazioni
  for (count=0; count<iterations && numHashesLeft > 0 ; ++count) {
    #if VERBOSE
      if (!--window) {
        end = get_real_time();      
        fprintf(stderr, "[info] analyzed passwords per second: %.2f\n", ((double)PERF_WINDOW)/(end-start));
        window = PERF_WINDOW;
        start = get_real_time();      
      }
    #endif    
    
    encode(alphabet, charsetLength, stringLength, count, base, buffer); // genera permutazione in buffer
    
    callback(buffer, stringLength, hash); // salva digest MD5 in hash
    for (i = 0; i < 16; i++)
      sprintf(&(str_hash[2*i]), "%2.2x", hash[i]); // hash convertito in una stringa    
    
    if (findAndRemove(hashes, str_hash, numHashesLeft) < numHashesLeft) {
      --numHashesLeft; // assumo che non ci siano duplicati in hashes
      printf("%s %s\n", str_hash, buffer);
    }
  } */
