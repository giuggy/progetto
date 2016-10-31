__kernel void encode_k (__global char* buffer, __global const char* alphabet, const long charsetLength, const long stringLength, long counter ) {

	int i = get_global_id(0);
	long c = counter;
	
	if(i>= stringLength) return;
	
	int j;
	for (j = 0; j < i; j++) c /= charsetLength;
	
    long a =  a / charsetLength + (c % charsetLength); // aggiorna cella j-esima    
    a -= (a / charsetLength) * charsetLength; // ... ma anche per quello corrente!       
    buffer[i] = alphabet[a]; // costruisce la stringa
    
    counter/= charsetLength;
}

//genera la permutazione counter-esima dati un alfabeto ed una base

