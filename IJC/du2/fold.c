/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/22/2014 08:22:18 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 1
 * =====================================================================
 */

/*
 * =====================================================================
 *                     Volna implementace fold v C
 * =====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "io.h"

#define DEFAULT_LENGTH 80
#define WORDBUFFER 4096

struct params {
	bool stdinFile;
	char * fileName;
	long lineLength;
	long maxLength;
	bool cflag;
};


int fold (struct params * forMe);
void printErr(const char * msg);

/*   Main
 * ---------------------------------------------------------------------
 * - zpracovava argumenty vola funkci fold
 */
int main (int argc, char * argv[])
{
	struct params forFold = { true, .lineLength=DEFAULT_LENGTH, WORDBUFFER - 1, false };
	char * end;

	if (argc >= 1 && argc <= 5)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp("-w", argv[i]) == EXIT_SUCCESS && i + 1 < argc) // parametr -w
			{
				forFold.lineLength = strtol(argv[++i], &end, 0);	// dalsi paramet je cislo
				if (*end) { printErr("Number expected after -w."); return EXIT_FAILURE; }
			} else if (strcmp("-c", argv[i]) == EXIT_SUCCESS) // parametr -c
			{
				forFold.cflag = true;
			} else if ( i < argc - 1) // neznamy paramer a neni to parametr posledni
			{
				printErr("Unrecognized option, exiting.");
				return EXIT_FAILURE;
			} else // mam posledni parametr == jmeno souboru
			{
				forFold.fileName = argv[i];
				forFold.stdinFile = false;
			}
		}
	} else
	{
		printErr("Something went wrong. Please use valid syntax.");
		return EXIT_FAILURE;
	}

	return fold(&forFold);
}

/*   Skladani radku
 * ---------------------------------------------------------------------
 * - funkci fgetw z io.h cte slova a postupne je vypisjue na stdout tak,
 * - aby nedoslo k prekroceni zadane delky radku (implic. 80)
 * - respektuje volne radky (jako volne radky povazuji i radky, ktere
 *   obsahuji pouze mezery a jine oddelovace - vizualne jsou prazdne)
 * - v pripade dlouhych slov je zkrati (v pripade prepinace -c) a
 *   vypise na samostatny radek
 */
int fold (struct params * forMe)
{
	
	FILE * input;
	if (forMe->stdinFile) { input = stdin; }
	else if  ((input = fopen(forMe->fileName, "r")) == NULL) { printErr("Unable to open file"); return EXIT_FAILURE; }

// na zacatku souboru jsou volne radky
	int ch;
	while ((ch = fgetc(input)) != EOF && ch == '\n') {putchar(ch); }
	ungetc(ch, input);

// zpracovani souboru
	char word[WORDBUFFER];
	int wordLength;
	int lineCounter = 0;
	int enterCounter = 0;
	bool err = false;
	while (( wordLength = fgetw(word, forMe->maxLength, input)) != 0)
	{
		enterCounter = 0;
		if (( lineCounter + wordLength) >= forMe->lineLength && lineCounter != 0 ) { putchar('\n'); lineCounter = 0; }
		if (!err && wordLength >= forMe->lineLength) {printErr("Word longer than line!"); err = true;}
		if (lineCounter != 0) { putchar(' '); lineCounter++; }
		if (forMe->cflag) {word[forMe->lineLength] = '\0'; }
		printf("%s", word);
		lineCounter += wordLength;
		while ((ch = fgetc(input)) != EOF && isspace(ch))
		{
			if (ch == '\n') {enterCounter++; }
		}
		ungetc(ch, input);
		if (enterCounter == 1) {continue; }
		while (enterCounter > 0) { putchar('\n'); enterCounter--; lineCounter = 0; }
	}
	putchar('\n');
	fclose(input);
	return EXIT_SUCCESS;
}
