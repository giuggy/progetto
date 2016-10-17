// genera la permutazione counter-esima dati un alfabeto ed una base
inline void encode(uint8_t* alphabet, size_t charsetLength, size_t stringLength, uint64_t counter, uint8_t* buffer ) {
  long j=0, a=0, carry=0;
  for ( ; j < stringLength; ++j, counter/= charsetLength) {    
    a = carry + (counter % charsetLength); // aggiorna cella j-esima    
    carry = a / charsetLength; // tiene conto del riporto per lo step successivo
    a -= carry * charsetLength; // ... ma anche per quello corrente!       
    buffer[j] = alphabet[a]; // costruisce la stringa
    
  }
}
