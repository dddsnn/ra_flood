CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = 

SRC = ra_flood.c ip6.c icmp6.c checksum.c
OBJ = ${SRC:%.c=%.o}
HDR = ${SRC:%.c=%.h}

all: ra_flood

ra_flood: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -f ra_flood ${OBJ}

.PHONY: all clean
