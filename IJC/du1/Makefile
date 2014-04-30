#
# =====================================================================
#          Verze:  1.0
#      Vytvoreno:  22.3.2014 19:03:52
#          Autor:  Tomas Coufal
#          Login:  xcoufa09
#        Projekt:  IJC - Projekt c.1
# =====================================================================
#

#
# ======================================================================
#                            Makefile
# ======================================================================

CC=gcc
CFLAGS= -std=c99 -g -O2 -pedantic -Wall

#
#   Navod pro all (a make bez parametru)
# ----------------------------------------------------------------------
all: prvocisla prvocisla-inline steg-decode

#
#   Navody pro jednotlive priklady
# ----------------------------------------------------------------------
prvocisla: prvocisla.o eratosthenes.o error.o
	$(CC) -lm -o $@ prvocisla.o eratosthenes.o error.o

prvocisla-inline: prvocisla-inline.o eratosthenes-inline.o error.o
	$(CC) -lm -o $@ prvocisla-inline.o eratosthenes-inline.o error.o

steg-decode: steg-decode.o eratosthenes.o error.o ppm.o
	$(CC) -lm -o $@ steg-decode.o error.o ppm.o eratosthenes.o

steg-decode-inline: steg-decode.o eratosthenes-inline.o error.o ppm.o
	$(CC) -lm -o $@ steg-decode.o error.o ppm.o eratosthenes.o

#
#   Navody pro zavislosti
# ----------------------------------------------------------------------
prvocisla.o: prvocisla.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c -o $@ prvocisla.c
prvocisla-inline.o: prvocisla.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE -o $@ prvocisla.c
eratosthenes.o: eratosthenes.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c -o $@ eratosthenes.c
eratosthenes-inline.o: eratosthenes.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE -o $@ eratosthenes.c
steg-decode.o: steg-decode.c ppm.h bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -c -o $@ steg-decode.c
error.o: error.c error.h
	$(CC) $(CFLAGS) -c -o $@ error.c
ppm.o: ppm.c ppm.h error.h
	$(CC) $(CFLAGS) -c -o $@ ppm.c

#   Uklid prosim!
# ----------------------------------------------------------------------
.SILENT:clean clean-all
clean:
	rm -f prvocisla prvocisla-inline steg-decode
clean-all:
	rm -f prvocisla prvocisla-inline steg-decode *.o
