#ifndef _IPV6_H
#define _IPV6_H

#include <stdint.h>

uint8_t *build_ip6pkt(uint8_t trafcls, uint32_t flow, uint8_t nxhd,
	uint8_t hoplim, uint16_t srcadd[8], uint16_t destadd[8],
	uint16_t payload_len);

uint8_t *build_ip6pkt_simple(uint8_t nxhd, uint16_t srcadd[8],
	uint16_t destadd[8], uint16_t payload_len);

uint8_t *build_ip6pkt_min(uint16_t payload_len);

void ip6_set_nxhd(uint8_t *buf, uint8_t nxhd);

void ip6_set_srcadd(uint8_t *buf, uint16_t srcadd[8]);

void ip6_set_destadd(uint8_t *buf, uint16_t destadd[8]);

void ip6_set_hoplim(uint8_t *buf, uint8_t hoplim);

#endif
