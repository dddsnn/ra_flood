#ifndef _RA_FLOOD_H
#define _RA_FLOOD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>


unsigned int validate_uint(char *str);

void ra_flood(unsigned int num, unsigned int delay, char ifc[IFNAMSIZ]);

/**
 * Bindet das raw socket s an Interface ifc mit Broadcastadresse.
 */
struct sockaddr_ll broadcast_socket(int s, char ifc[IFNAMSIZ]);

void print_help();

#endif
