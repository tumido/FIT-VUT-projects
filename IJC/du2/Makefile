#
#  =====================================================================
#           Verze:  1.0
#       Vytvoreno:  04/27/2014 10:46:48 AM
#           Autor:  Tomáš Coufal
#           Login:  xcoufa09
#         Projekt:  IJC - Projekt c. 2 - Priklad 2
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
CFLAGS = -std=c99 -pedantic -Wall -Werror
CPPFLAGS = -std=c++11 -pedantic -Wall -Werror
PROGS = fold fold2 wordcount wordcount-dynamic

#    Pravidla pro jednotlive programy
#  ---------------------------------------------------------------------
all: $(PROGS)

fold: fold.c io.o
	$(CC) $(CFLAGS) $^ -o $@

fold2: fold.cc
	$(CPP) $(CPPFLAGS) $< -o $@

wordcount: wordcount.c htable.a io.o
	$(CC) $(CFLAGS) -static $^ -o $@

wordcount-dynamic: wordcount.c htable.so io.o
	$(CC) $(CFLAGS) $^ -o $@

#    Pravidla pro knihovny
#  ---------------------------------------------------------------------
htable.a: htable-clear.o htable-foreach.o htable-free.o htable-hash.o htable-init.o htable-lookup.o htable-remove.o htable-statistics.o
	ar -rsc $@ $^

htable.so: dynamic-htable-clear.o dynamic-htable-foreach.o dynamic-htable-free.o dynamic-htable-hash.o dynamic-htable-init.o dynamic-htable-lookup.o dynamic-htable-remove.o dynamic-htable-statistics.o
	$(CC) -shared -fPIC $^ -o $@

#    Ostatni objekty
#  ---------------------------------------------------------------------
dynamic-%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

#    Doplnkove funkce
#  ---------------------------------------------------------------------
clean:
	rm -vf *.o *.a *.so $(PROGS)

zip:
	zip xcoufa09.zip *.c *.cc *.h Makefile

# $@ - (cil) jmeno vystupu
# $< - (zdroj) vstup
# gcc -MM *.c
