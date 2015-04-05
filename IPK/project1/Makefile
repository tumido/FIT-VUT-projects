#
#  =====================================================================
#           Verze:  1.0
#       Vytvoreno:  04/03/2015 17:50:49 PM
#           Autor:  Tomáš Coufal
#           Login:  xcoufa09
#         Projekt:  IPK - Projekt c. 1
#  =====================================================================
#

#
#  =====================================================================
#                               Makefile
#  =====================================================================
#

#    Deklarace promenych
#  ---------------------------------------------------------------------
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Werror
PROGS = client server
#    Pravidla pro jednotlive programy
#  ---------------------------------------------------------------------
all: $(PROGS)

server: server.c io.o
	$(CC) $(CFLAGS) -pthread server.c io.o -o $@

client: client.c io.o
	$(CC) $(CFLAGS) client.c io.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

#    Doplnkove funkce
#  ---------------------------------------------------------------------
clean:
	rm -vf *.o *.a *.so $(PROGS)

zip:
	zip xcoufa09.zip *.c Makefile

# $@ - (cil) jmeno vystupu
# $< - (zdroj) vstup
# gcc -MM *.c
