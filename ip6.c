#include "ip6.h"
#include "checksum.h"

#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/**
 * Baut ein IPv6-Paket
 * trafcls: 8b Traffic Class
 * flow: 20b Flow Label, höherwertige Bits werden ignoriert
 * nxhd: 8b Typ des nächste Headers
 * hoplim: 8b Hop Limit
 * srcadd: 8*16b Absenderadresse
 * destadd: 8*16b Zieladresse
 * payload_len: Länge des Payloads in Byte
 * Gibt einen Buffer zurück, in dem die ersten 40 Bytes der IPv6-Header sind und
 * noch payload_len Bytes Platz ist oder NULL, wenn kein Speicher da war.
 */
uint8_t *build_ip6pkt(uint8_t trafcls, uint32_t flow, uint8_t nxhd,
	uint8_t hoplim, uint16_t srcadd[8], uint16_t destadd[8],
	uint16_t payload_len){
	uint8_t *buf = (uint8_t*)malloc(40+payload_len);
	if(!buf)
		return NULL;
	//ip version
	buf[0] = (6 << 4);
	//traffic class
	buf[0] |= (trafcls >> 4);
	buf[1] = (trafcls << 4);
	//flow label
	buf[1] |= (flow >> 16);
	buf[2] = (flow >> 8);
	buf[3] = flow;
	//payload length
	buf[4] = (payload_len >> 8);
	buf[5] = payload_len;
	//ip next header
	buf[6] = nxhd;
	//hop limit
	buf[7] = hoplim;
	//source address
	for(int i=0; i<8; i++){
		buf[2*i+8] = (srcadd[i] >> 8);
		buf[2*i+9] = srcadd[i];
	}
	//destination address
	for(int i=0; i<8; i++){
		buf[2*i+24] = (destadd[i] >> 8);
		buf[2*i+25] = destadd[i];
	}
	return buf;
}

/**
 * Vereinfachtes IPv6-Paket, hierbei ist
 * Traffic Class = 0;
 * Flow Label = 0;
 * Hop Limit = 42;
 */
uint8_t *build_ip6pkt_simple(uint8_t nxhd, uint16_t srcadd[8],
	uint16_t destadd[8], uint16_t payload_len){
	return build_ip6pkt(0, 0, nxhd, 42, srcadd, destadd, payload_len);
}

/**
 * Baut ein IPv6-Paket, das bis auf die Version und die Payload Length nur
 * Nullen enthält.
 * payload_len: Größe des Speichers in Bytes, der zusätzlich reserviert wird und
 * 	bereits in den Header geschrieben wird
 * Gibt einen Buffer zurück, der 40+payload_len Bytes groß ist oder NULL, wenn
 * kein Speicher da war.
 */
uint8_t *build_ip6pkt_min(uint16_t payload_len){
	uint8_t *buf = malloc(40+payload_len);
	if(!buf)
		return NULL;
	bzero(buf, 40+payload_len);
	buf[0] = (6 << 4);
	buf[4] = (payload_len >> 8);
	buf[5] = payload_len;
	return buf;
}

/**
 * Setzt in einem vorhandenen IPv6-Paket das Next-Header-Feld auf nxhd
 * buf: Buffer, der das Paket enthält, muss mindestens 40 Bytes groß sein
 * nxhd: Wert, der für next header gesetzt werden soll
 */
void ip6_set_nxhd(uint8_t *buf, uint8_t nxhd){
	buf[6] = nxhd;
}

/**
 * Setzt in einem vorhandenen IPv6-Paket die Absenderadresse neu
 * siehe ip6_set_nxhd()
 */
void ip6_set_srcadd(uint8_t *buf, uint16_t srcadd[8]){
	for(int i=0; i<8; i++){
		buf[2*i+8] = (srcadd[i] >> 8);
		buf[2*i+9] = srcadd[i];
	}
}

/**
 * Setzt in einem vorhandenen IPv6-Paket die Zieladresse neu
 * siehe ip6_set_nxhd
 */
void ip6_set_destadd(uint8_t *buf, uint16_t destadd[8]){
	for(int i=0; i<8; i++){
		buf[2*i+24] = (destadd[i] >> 8);
		buf[2*i+25] = destadd[i];
	}
}

/**
 * Setzt in einem vorhandenen IPv6-Paket das Hop Limit neu
 * siehe ip6_set_nxhd
 */
void ip6_set_hoplim(uint8_t *buf, uint8_t hoplim){
	buf[7] = hoplim;
}

