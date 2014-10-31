#include "icmp6.h"
#include "checksum.h"

#include <stdlib.h>
#include <strings.h>
#include <time.h>

/**
 * Hängt ein ICMPv6 Router-Advertisement-Paket an ein bestehendes IPv6-Paket
 * buf: Buffer, der in den ersten 40 Bytes einen IPv6-Header enthält und danach
 * 	noch 16+opts_len Bytes Platz hat
 * opts_len: Länge der Optionen, ipv6 muss noch genug Platz dafür haben
 */
void append_icmp6_ra(uint8_t *buf, uint8_t opts_len){
	bzero(buf+40, 16+opts_len);
	//router advertisement
	buf[40] = 134;
	//code bleibt 0
	//checksum später
	//empfohlenes hop limit
	buf[44] = 1;
	//flags: high priority
	buf[45] = 8;
	//maximale lifetime
	buf[46] = buf[47] = ~0;
	//maximale timeouts
	for(int i=48; i<56; i++)
		buf[i] = ~0;
	//checksum
	icmp6_calc_checksum(buf, 56+opts_len);
}
//wird opts_len da egtl benutzt??


/**
 * Berechnet im ICMPv6-Paket buf der Länge len die neue Checksum.
 * buf: ICMPv6-Paket komplett mit IPv6-Header und allem
 */
void icmp6_calc_checksum(uint8_t *buf, uint16_t len){
	uint16_t sum;
	//checksum-feld auf 0 setzen
	buf[42] = buf[43] = 0;
	//hop limit durch next header ersetzen, gehört nicht in die checksum
	uint8_t hoplim = buf[7];
	buf[7] = buf[6];
	buf[6] = 0;
	sum = checksum(buf+4, len-4);
	buf[42] = (sum >> 8);
	buf[43] = sum;
	//hop limit <-> next header rückgängig
	buf[6] = buf[7];
	buf[7] = hoplim;
}

/**
 * Baut eine ICMPv6-Option Prefix Information an ein bestehendes Paket an.
 * buf: ICMPv6-Paket, in dem nach pos noch 32 Byte Platz für die Option sind
 * pos: Position, an der die Option angefügt wird (beginnt bei buf[pos])
 */
void append_icmp6_prefinf(uint8_t *buf, uint16_t pos){
	//type
	buf[pos] = 3;
	//length in 8 bytes
	buf[pos+1] = 4;
	//prefix length
	buf[pos+2] = 64;
	//l-, a-bits
	buf[pos+3] = 0;
	//valid, preferred lifetime
	for(int i=4; i<12; i++)
		buf[pos+i] = 0xff;
	//prefix
	srand(time(NULL));
	for(int i=16; i<32; i++)
		buf[pos+i] = rand();
}

/**
 * Baut eine ICMv6-Option MTU an ein bestehendes Paket an.
 * buf: ICMPv6-Paket, in dem noch 8 Byte Platz sind
 * pos: Position, an der die neue Option anfangen soll
 */
void append_icmp6_mtu(uint8_t *buf, uint16_t pos){
	//type
	buf[pos] = 5;
	//length in 8 bytes
	buf[pos+1] = 1;
	//mtu
	buf[pos+6] = (1500>>8);
	buf[pos+7] = (uint8_t)1500;
}

/**
 * Baut eine ICMPv6-Option Source Link-Layer Address an ein bestehendes Paket an
 * buf: ICMPv6-Paket, in dem noch 8 Byte Platz sind
 * pos: Position, an der die neue Option anfangen soll
 */
void append_icmp6_slla(uint8_t *buf, uint16_t pos){
	//type
	buf[pos] = 1;
	//length in 8 bytes
	buf[pos+1] = 1;
	//address
}
