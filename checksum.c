#include "checksum.h"

#include <stdint.h>

/**
 * Bildet die Prüfsumme des Buffers buf der Länge buf_len und gibt sie zurück.
 */
uint16_t checksum(uint8_t *buf, unsigned int buf_len){
	uint_fast32_t sum = 0;
	while(buf_len>1){
		sum += (*buf<<8 | *(buf+1));
		buf += 2;
		buf_len -= 2;
	}
	//bei ungerader Anzahl
	if(buf_len)
		sum += *buf<<8;
	//carrys aufsummieren
	while(sum>>16)
		sum = (sum & 0xffff) + (sum>>16);
	return (uint16_t)~sum;
}
