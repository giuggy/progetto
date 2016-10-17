__kernel void encode (__global uint8_t* buffer, __global const uint8_t* alphabet, __global const size_t charsetLength, __global const size_t stringLength, __global const uint64_t counter ) {
 

	int i = get_global_id(0);
	if (i = stringLength){
		buffer[i]='\0';
		return;
	}
	if(i> stringLength) return;
	

	int a=0; 
	int carry=0;
	counter= (counter+i)/ charsetLength
	a = carry + (counter % charsetLength);
	carry = a / charsetLength;
	a -= carry * charsetLength;
	buffer[i] = alphabet[a];


}

//genera la permutazione counter-esima dati un alfabeto ed una base

