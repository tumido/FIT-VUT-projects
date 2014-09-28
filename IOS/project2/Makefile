#
#  =====================================================================
#           Verze:  1.0
#       Vytvoreno:  04/27/2014 10:46:48 AM
#           Autor:  Tomáš Coufal
#           Login:  xcoufa09
#         Projekt:  IOS - Projekt c. 2
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
CPP = g++
CFLAGS = -g -std=gnu99 -pedantic -Wall -lpthread -lrt
PROGS = rivercrossing

all: $(PROGS)

#    Pravidla pro jednotlive programy
#  ---------------------------------------------------------------------
rivercrossing: rivercrossing.c generator.o sync.o crew.o io.o
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

#    Doplnkove funkce
#  ---------------------------------------------------------------------
clean:
	rm -vf *.o *.a *.so $(PROGS)

zip:
	zip xcoufa09.zip *.c *.cc *.h Makefile
