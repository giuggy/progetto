__kernel void  (__global const char *hashes, __global int *retu, __global const char *s, size_t numLines) {
 
	// confronto tra due stringhe
	int i = get_global_id(0);

	if (i >= numLines) return;
	int k;	
	int flag=0;
	for (k = 0; k< 33; k++){
	
		if (hashes[i * 33 + k] == s[k]) retu[k] = 0;
		else{
			retu[k] = 1;
			flag=1;
		}
	}
	if(!flag) return;

}

