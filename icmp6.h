#ifndef _ICMPV6_H
#define _ICMPV6_H

#include <stdint.h>

void append_icmp6_ra(uint8_t *buf, uint8_t opts_len);

void icmp6_calc_checksum(uint8_t *buf, uint16_t len);

void append_icmp6_prefinf(uint8_t *buf, uint16_t pos);

void append_icmp6_mtu(uint8_t *buf, uint16_t pos);

void append_icmp6_slla(uint8_t *buf, uint16_t pos);

#endif
