/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  24.11.2013 21:01:44
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c. 2
 *         Soubor:  proj2.c
 * =====================================================================
 */

/*
 * =====================================================================
 *    Program pro vypocet arcus sinu a druhe odmocniny z daneho cisla
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define EPSILON 1e-12
#define NAN 0.0/0.0
#define PI 3.14159265358979323846

/*   Struktura souradnic trojuhelnika
 * ---------------------------------------------------------------------
 * - struktura uchovavajici souradnice vrcholu trojuhelika
 * - zaroven dovoluje prochazet strukturou jako polem a pristupovat
 *   k jednotlivym souradnicim pomoci indexu (vhdne k iteracim) skrz
 *   strukturu
 */
typedef union cUnion
{
	struct cStruct
	{
		double ax;
		double ay;
		double bx;
		double by;
		double cx;
		double cy;
	} cStruct;
	double cArray[6];
} Tcoords;

/*   Prototypy vytvorenych funkci
 * ---------------------------------------------------------------------
 */
int parseMode(int argc, char * argv[], bool * sqrt, bool * asin, bool * triangle);
int parseValueTriangle(int argc, char * argv[], Tcoords * c);
int parseValueAsinSqrt(int argc, char * argv[], double * number);
int validTriangle(Tcoords * c);
void my_triangle(Tcoords * c, double * angle);
double my_asin(double x);
double my_sqrt(double x);
double my_acos(double x);
double asinOpt(double x);
double square(double x);
double fabs(double x);
void printErr(char * msg);
void printHelp();

/*   Hlavni program
 * ---------------------------------------------------------------------
 * - necha zpracovat argumenty pro dany mod a nasledne zavola
 *   pozadovanou funkci
 * - pohlida spravny prubeh a vypise vysledky
 */
int main(int argc, char * argv[])
{
	bool sqrtMode = false;
	bool asinMode = false;
	bool triangleMode = false;
	double angle[3];
	double number;
	Tcoords coordinates;

	if (parseMode(argc, argv, &sqrtMode, &asinMode, &triangleMode) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if (triangleMode == true && parseValueTriangle(argc, argv, &coordinates) == EXIT_SUCCESS)
	{
		my_triangle(&coordinates, angle);
		for (int i=0; i <= 2; i++)
		{
			printf("%.10e\n", angle[i]);
		}
	}
	else if (sqrtMode == true && parseValueAsinSqrt(argc, argv, &number) == EXIT_SUCCESS)
	{
		printf("%.10e\n", my_sqrt(number));
	}
	else if (asinMode == true && parseValueAsinSqrt(argc, argv, &number) == EXIT_SUCCESS)
	{
		printf("%.10e\n", asinOpt(number));
	} else
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*   Uhly v trojuhelniku
 * ---------------------------------------------------------------------
 * - nejdrive si spocita delky jednotlivych stran trojuhelnika, ty ulozi
 *   do pole size
 * - pak pomoci kosinove vety postupne spocita velikosti uhlu prislusici
 *   jednotlivy stranam
 */
void my_triangle(Tcoords * c, double * angle)
{
	double side[3]; /* pole tri hodnot pro delky stran */

	if (validTriangle(c) != EXIT_SUCCESS)
	{
		for (int i = 0; i <= 2; i++)
		{
			angle[i] = NAN;
		}
	} else
	{
		side[0] = my_sqrt(square(fabs((*c).cStruct.bx - (*c).cStruct.cx)) + square(fabs((*c).cStruct.by - (*c).cStruct.cy)));
		side[1] = my_sqrt(square(fabs((*c).cStruct.cx - (*c).cStruct.ax)) + square(fabs((*c).cStruct.cy - (*c).cStruct.ay)));
		side[2] = my_sqrt(square(fabs((*c).cStruct.ax - (*c).cStruct.bx)) + square(fabs((*c).cStruct.ay - (*c).cStruct.by)));

		angle[0] = my_acos((square(side[1]) + square(side[2]) - square(side[0])) / (2 * side[1] * side[2]));
		angle[1] = my_acos((square(side[2]) + square(side[0]) - square(side[1])) / (2 * side[2] * side[0]));
		angle[2] = my_acos((square(side[0]) + square(side[1]) - square(side[2])) / (2 * side[0] * side[1]));
	}
}

/*   Arcus Sinus
 * ---------------------------------------------------------------------
 * - vypocet hodnoty arcus sinu z cisla x
 * - pouziva aproximaci pomoci Taylorovy rady
 * - pocita s presnosti hodnoty EPSILON
 * - vraci arcus sinus z cisla x
 */
double my_asin(double x)
{
	unsigned long i = 1;
	double member = x;
	double squareX = square(x);
	double result = member;

	if (x < -1.0 || x > 1.0)
	{
		return NAN;
	}

	while (fabs(member) > EPSILON * fabs(result))
	{
		i += 2;
		member = member * ((i - 2.0) * (i - 2.0) * squareX) / ((i - 1.0) * i);
		result += member;
	}

	return result;
}

double asinOpt(double x)
{
	double result;
	double squareX = square(x);
	if (x >= -1.0 && x <= -0.5)
	{
		result = -PI / 2 + my_asin(my_sqrt(1 - squareX));
	}
	else if (x >= 0.5 && x <= 1.0)
		result = PI / 2 - my_asin(my_sqrt(1 - squareX));
	else
	{
		result = my_asin(x);
	}
	return result;
}

/*   2. odmocnina
 * ---------------------------------------------------------------------
 * - iteracni metodou se dopocitava druhe odmocniny z daneho cisla
 */
double my_sqrt(double x)
{
	double result = 0;
	double resultNew = 1;

	if (x < 0)
	{
		return NAN;
	}

	if (x < EPSILON)
	{
		return 0;
	}

	while (fabs(resultNew - result) > EPSILON * fabs(resultNew))
	{
		result = resultNew;
		resultNew = 0.5 * (x / result + result);
	}
	return resultNew;
}

/*  Arcus Cosinus
 * ---------------------------------------------------------------------
 * - prepocet arcus sinu na arcus cosinus
 */
double my_acos(double x)
{
	if (x < -1.0 || x > 1.0)
	{
		return NAN;
	} else
	{
		return PI / 2 - asinOpt(x);
	}
}

/*   Druha mocnina
 * ---------------------------------------------------------------------
 * - vraci druhou mocninu cisla x
 */
double square(double x)
{
	return x*x;
}

/*   Absolutni hodnota
 * ---------------------------------------------------------------------
 * - vypocita absolutni hodnotu z desetinneho cisla
 */
double fabs(double x)
{
	return x >= 0 ? x : -x;
}

/*   Zjisteni modu
 * ---------------------------------------------------------------------
 * - zpracuje prepinace a zajisti zavolani napovedy
 * - v pripade, ze nebyl zadan zadny argument nebo byl argument neplatny
 *   vrati EXIT_FAILURE
 */
int parseMode(int argc, char * argv[], bool * sqrt, bool * asin, bool * triangle)
{
	if (argc == 1)
	{
		printErr("Nebyl zadan zadny argument...");
		return EXIT_FAILURE;
	}
	
	/* podiva se na libovolny argument, jestli neni pozadovan Help (pocitam od
	 * i = 1,  protoze argv[0] neni parametr), zavola printHelp() a skonci */
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == EXIT_SUCCESS || strcmp(argv[i], "-h") == EXIT_SUCCESS)
		{
			printHelp();
			return EXIT_SUCCESS;
		}
	}

	if (strcmp(argv[1], "--sqrt") == EXIT_SUCCESS)
	{
		*sqrt = true;
	}
	else if (strcmp(argv[1], "--asin") == EXIT_SUCCESS)
	{
		*asin = true;
	}
	else if (strcmp(argv[1], "--triangle") == EXIT_SUCCESS)
	{
		*triangle = true;
	}
	else
	{
		printErr("Neznama volba!");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*   Zpracovani argumentu pro trojuhelnik
 * ---------------------------------------------------------------------
 * - prochazi polem argumentu a postupne je prevadi na cisla, ktera
 *   prirazuje do pole (struktury) souradnic
 * - zavola overeni, jestli trojuhelnik existuje, pokud ne, vypise NAN
 *   pro kazdy uhel
 * - pokud vse probehne v poradku vraci EXIT_SUCCESS
 */
int parseValueTriangle(int argc, char * argv[], Tcoords * c)
{
	char * tail = NULL;
	
	if (argc != 8)
	{
		printErr("Neuplne zadani.");
		return EXIT_FAILURE;
	}

	for (int i = 2; i <= 7; i++)
	{
		tail = argv[i];
		(*c).cArray[i-2] = strtod(argv[i], &tail);
		if (strcmp(tail, "") != EXIT_SUCCESS)
		{
			printErr("Souradnice nejsou cisla.");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*   Zpracovani argumentu pro vypocet odmocniny
 * ---------------------------------------------------------------------
 * - zjisti, jestli je zadan spravny pocet argumentu
 * - overi, zda-li se jedna o cislo
 * - pokud to cislo je, tak overi, zda-li nalezi do intervalu, ve kterem
 *   je definovana 2. odmocnina - pokud neni vypisuje NAN
 */
int parseValueAsinSqrt(int argc, char * argv[], double * number)
{
	char * tail = argv[2];
	
	if (argc != 3)
	{
		printErr("Spatny pocet argumentu.");
		return EXIT_FAILURE;
	}
	
	*number = strtod(argv[2], &tail);
	if (strcmp(tail, "") == EXIT_SUCCESS)
	{
		return EXIT_SUCCESS;
	} else
	{
		printErr("Nejedna se o cislo");
		return EXIT_FAILURE;
	}
}

/*   Overeni trojuhelniku
 * ---------------------------------------------------------------------
 * - overi jestli se jedna o trojuhelnik
 * - zkontroluje zda-li je vektor A->B nasobkem A->C (tj. jsou-li body
 *   v primce)
 * - dale overi zda-li nejsou netere dva body jednim
 */
int validTriangle(Tcoords * c)
{
	if ((fabs((((*c).cStruct.bx - (*c).cStruct.ax)/((*c).cStruct.by - (*c).cStruct.ay)) - fabs(((*c).cStruct.cx - (*c).cStruct.ax)/((*c).cStruct.cy - (*c).cStruct.ay))) < EPSILON) ||
		 ((fabs((*c).cStruct.ax - (*c).cStruct.cx) < EPSILON) && (fabs((*c).cStruct.ay - (*c).cStruct.cy) < EPSILON) ) ||
		 ((fabs((*c).cStruct.ax - (*c).cStruct.bx) < EPSILON) && (fabs((*c).cStruct.ay - (*c).cStruct.by) < EPSILON) ))
	{
		return EXIT_FAILURE;
	} else
	{
		return EXIT_SUCCESS;
	}
}

/*  Napoveda a vypis chyboveho hlaseni
 * ---------------------------------------------------------------------
 * - funkce printErr() vypisuje chybove hlaseni do stderr a nabidne
 *   syntaxi pro vypsani napovedy
 * - funkce printHelp() vypisuje podrobnou napovedu
 */
void printErr(char * msg)
{
	fprintf(stderr, "%s\n", msg);
	printf("Pro ziskani napovedy pouzijte: ./proj2 --help\n");
}
void printHelp()
{
	printf("Program pocita arcus sinus, druhou odmocninu ci vnitrni uhly v trojuhelniku.\n");
	printf("Spousteni: ./proj2 VOLBA HODNOTA\n\n");
	printf("Volby:\n");
	printf("  --sqrt                    vypocita druhou odmocnimu z cisla HODNOTA\n");
	printf("  --asin                    vypocita arcus sinus z cisla HODNOTA\n");
	printf("  --triangle                vypocita velikosti vnitrnich uhlu v trojuhelniku\n");
	printf("  -h  --help                vypise tuto napovedu a skonci\n\n");
	printf("Parametr HODNOTA muze byt:\n");
	printf("  pro druhou odmocninu:     desetinne cislo vetsi nebo rovno 0\n");
	printf("  pro arcus sinu:           desetinne cislo v intervalu <-1; 1>\n");
	printf("  pro vypocet uhlu:         souradnice vrcholu v poradi AX AY BX BY CX CY\n\n");
	printf("Autor: Tomas Coufal  <xcoufa09@stud.fit.vutbr.cz>\n");
	printf("Login: xcoufa09\n");
}
