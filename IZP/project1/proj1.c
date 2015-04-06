/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  30.10.2013 16:51:30
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c. 1
 *         Soubor:  proj1.c
 * =====================================================================
 */

/*
 * =====================================================================
 *        Program pocita slova obsahujici dany znak ci cislici
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h> /* konstanty EXIT_SUCCESS a EXIT_FAILURE */
#include <string.h> /* funkce strcmp(), strlen() */
#include <limits.h> /* konstanta INT_MAX */

#define MAX_DEBUG 80 /* pocet pismen slova vypisovanych Debug modem*/
#define BASE 10 /* rad cis. soustavy u prevodu param. UMISTENI na int*/
#define NOT_NUMBER 0 /* navratova hodnota, kdyz selze prevod UMISTENI */
#define FALSE 0
#define TRUE 1

/*   Prototypy vytvorenych funkci
 * ---------------------------------------------------------------------
 */
void printHelp();
void printErr(char * msg);
int findPattern(int pattern, int index, int debugMode);
int evaluateMatch(int pattern, int letter, int index, int counterPosition, int wasItCounted);
int stringToNumber(char * ch);
int matchCharWithPattern(int pattern, int ch);
int legalChar(int ch);
void printForDebug(int letter, int * stateEnter, int couter);

/*   Main
 * ---------------------------------------------------------------------
 *  - funguje jako wrapper pro findPattern() a zajistuje zpracovani
 *    argumentu
 *  - kontroluje pocet argumentu a jejich spravny format a umisteni
 *  - pokud je argument spatne zadany nebo chybi povinny argument,
 *    necha vypsat chybove hlaseni
 */
int main(int argc, char * argv[])
{
  int i, pattern;
  int debugMode = 0;
  int index = 0;

  /*
   * Testuje, jestli byly programu predany nejake argumenty.
   * Pokud nebyly nebo jich bylo prilis mnoho, program vypise
   * chybove hlaseni a skonci.
   */

  /* argc == 1 => program nema zadny argument */
  if (argc == 1)
  {
    printErr("Nebyl zadan zadny argument...");
    return EXIT_FAILURE;
  }
  /* argc > 4 => maximalni pocet argumentu je 3 (argc je maximalne rovno 4)*/
  if (argc > 4)
  {
    printErr("Prilis mnoho argumentu...");
    return EXIT_FAILURE;
  }

  /* Zpracovani argumentu */

  /* podiva se na libovolny argument, jestli neni pozadovan Help (pocitam od
   * i = 1,  protoze argv[0] neni parametr), zavola printHelp() a skonci */
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == EXIT_SUCCESS || strcmp(argv[i], "-h") == EXIT_SUCCESS)
    {
      printHelp();
      return EXIT_SUCCESS;
    }
  }

  /* podiva se na posledni dva argumenty, jestli neni urceno poradi hledaneho
   * znaku ve slove ci pozadovan Debug Mode, pokud je argunemt nesmyslny vrati
   * chybove hlaseni */
  for (i = 2; i < argc; i++)
  {
    if (strcmp(argv[i], "-d") == EXIT_SUCCESS)
    {
      debugMode = TRUE;
    } else
    {
      if ((index = stringToNumber(argv[i])) == NOT_NUMBER )
      {
        printErr("Volitelny argument neni platny!");
        return EXIT_FAILURE;
      }
    }
  }

  /* prevede prvni argument (hledany znak) na cislo a zkontroluje jestli je
   * validnim ci zastupnym znakem, v pripade ze neni ci se nejedna
   * o jeden znak vrati chybove hlaseni */
  if (strlen(argv[1]) == 1)
  {
    pattern = argv[1][0];
    if (legalChar(pattern) != EXIT_SUCCESS && pattern != '.' && pattern != ':' && pattern != '^')
    {
      printErr("Argument ZNAK neni platny!");
      return EXIT_FAILURE;
    }
  } else
  {
    printErr("Argument ZNAK neni platny!");
    return EXIT_FAILURE;
  }

  /* volani funkce, ktera obstarava vyhledani */
  printf("%d\n", findPattern(pattern, index, debugMode));

  return EXIT_SUCCESS;
}

/*   Definice funkci
 * ---------------------------------------------------------------------
 */

/*   Rozhodnuti o zapocitani slova
 * ---------------------------------------------------------------------
 * - funkce rozhoduje, jestli se prave prochazene slovo zapocita nebo ne
 * - necha porovnat ctene pismeno s hledanym znakem, zjisti, jestli bylo
 *   slovo jiz zapocitano a pripadne, zda jsme na spravne pozici
 */
int evaluateMatch(int pattern, int letter, int index, int counterPosition, int wasItCounted)
{
  if (index != FALSE)
  {
    return matchCharWithPattern(pattern, letter) == EXIT_SUCCESS && counterPosition == index && wasItCounted == FALSE;
  } else
  {
    return matchCharWithPattern(pattern, letter) == EXIT_SUCCESS && wasItCounted == FALSE;
  }
}

/*   Hledani znaku ve slove
 * ---------------------------------------------------------------------
 * - provadi prochazeni slovem
 * - slovo cte po pismenu a kazde pismeno pokud je platnym znakem necha
 *   porovnat s hledanym znakem
 * - pocita pocet pismen ve slove, tak aby bylo zajisteno, ze pokud
 *   chceme porovnavat pismeno na urcite pozice ve slove, ze je tato
 *   pozice spravna
 * - vraci pocet nalezenych shod
 */
int findPattern(int pattern, int index, int debugMode)
{
  int counterFound = 0;
  int counterPosition = 1;
  int letter;
  int wasItCounted = FALSE;
  int stateEnter = TRUE;
  
  while ((letter = getchar()) != EOF)
  {
    if (debugMode == TRUE)
    {
      /* vypis v debug mode */
      printForDebug(letter, &stateEnter, counterPosition);
    }
    if (legalChar(letter) == EXIT_SUCCESS)
    {
      if (evaluateMatch(pattern, letter, index, counterPosition, wasItCounted) != EXIT_SUCCESS)
      {
        counterFound++;
        wasItCounted = TRUE;
      } else
      {
        counterPosition++;
      }
    } else
    {
      wasItCounted = FALSE;
      counterPosition = 1;
    }
  }
  return counterFound;
}

/*   Vypis v Debug Mode
 * ---------------------------------------------------------------------
 * - zajistuje vypis slov v Debug mode
 * - pokud je slovo kratsi nez 80 znaku tak jej normalne vypise, jinak
 *   na 81. znaku zalomi radek a dal si slova nevsima
 * - pokud je za sebou vice oddelovacu pomoci promenne stateEnter
 *   zjisti, zdali byl vypsan enter a zajisti, aby se na novy radek
 *   psalo az dalsi slovo
 */
void printForDebug(int letter, int * stateEnter, int counter)
{
  if (counter <= MAX_DEBUG)
  {
    if (legalChar(letter) == EXIT_SUCCESS)
    {
      putchar(letter);
      *stateEnter = FALSE;
    } else
    { 
      if( *stateEnter == FALSE)
      {
        putchar('\n');
      }
      *stateEnter = TRUE;
    }
  }
  if (counter == MAX_DEBUG + 1)
  {
    putchar('\n');
  }
}

/*   Porovnani pismene a hledaneho znaku
 * ---------------------------------------------------------------------
 * - porovna, jestli se hledany znak rovna prave ctenemu pismenu
 * - v pripade zastupnych znaku porovna ctene pismeno s rozsahem, ktery
 *   dany zastupny znak zastupuje
 */
int matchCharWithPattern(int pattern, int ch)
{
  if ((ch == pattern) ||
      (pattern == ':' && ch >= '0' && ch <= '9') ||
      (pattern == '^' && ch >= 'A' && ch <= 'Z') ||
      (pattern == '.'))
  {
    return EXIT_SUCCESS;
  } else
  {
    return EXIT_FAILURE;
  }
}

/*   Porovnani znaku s povolenymi znaky
 * ---------------------------------------------------------------------
 * - zjisti, jestli je prave cteny znak pismenem ci cislici nebo
 *   oddelovacem slov
 * - porovna znak s povolenymi rozsahy, tzn. jestli nalezi do (0..9),
 *   (a..z), (A..Z) nebo je to pomlcka ci podtrzitko
 */
int legalChar(int ch)
{
  if ((ch >= '0' && ch <= '9') ||
      (ch >= 'A' && ch <= 'Z') ||
      (ch >= 'a' && ch <= 'z') ||
      (ch == '-') ||
      (ch == '_'))
  {
    return EXIT_SUCCESS;
  } else
  {
    return EXIT_FAILURE;
  }
}

/*   Prevod pismene na cislo
 * ---------------------------------------------------------------------
 * - funkce prochazi retezcem od nejvyssiho radu k nejnizsimu a kazde
 *   jednotlive pismeno prevadi na cislo tak, ze od ascii hodnoty znaku
 *   odecte hodnotu znaku '0' a vynasobi jej radem, pak jej pricte k
 *   vyslednemu cislu
 * - rad je urcen konstantou BASE
 * - pokud retezec neni kladne cislo v desitkove soustave vraci
 *   konstantu NOT_NUMBER
 * - osetruje preteceni integeru (overuje, zda-li je cislo po pricteni
 *   vyssiho radu vetsi, nez predtim, pokud ne vraci 0)
 */
int stringToNumber(char * ch)
{
  int number = 0;
  int aduct = 0;
  for (int i = 0; ch[i] != '\0'; i++)
  {
    if (ch[i] >= '0' && ch[i] <= '9')
    {
      aduct = ch[i] - '0';
      if (number <= (INT_MAX - aduct) / BASE )
      {
        number = (number * BASE) + aduct;
      } else
      {
        return NOT_NUMBER;
      }
    } else
    {
      return NOT_NUMBER;
    } 
  }
  printf("%d", number);
  return number;
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
  printf("Pro ziskani napovedy pouzijte: ./proj1 --help\n");
}
void printHelp()
{
  printf("Program pocita pocet slov obsahujici hledany znak ci cislici.\n");
  printf("Spousteni: ./proj1 ZNAK [UMISTENI] [-d]\n\n");
  printf("Argumenty:\n");
  printf("  ZNAK               hledany znak nebo cislice\n");
  printf("  UMISTENI           volitelny argument, ktery urcuje pozadovanou pozici znaku\n");
  printf("                     ve slove, defaultne na pozici nezalezi, pozice prvniho znaku je 1\n\n");
  printf("  -d                 zapne tzv. Debug Mode, kdy je kazde prosle slovo vypisovano\n");
  printf("  -h  --help         vypise tuto napovedu a skonci\n\n");
  printf("Hledanym znakem muze byt:\n");
  printf("  (A..Z)             velke pismeno anglicke abecedy\n");
  printf("  (a..z)             male pismeno anglicke abecedy\n");
  printf("  (0..9)             arabska cislice\n");
  printf("  :                  libovolna cislice\n");
  printf("  ^                  libovolne velke pismeno\n");
  printf("  .                  libovolny znak\n\n");
  printf("Autor: Tomas Coufal  <xcoufa09@stud.fit.vutbr.cz>\n");
  printf("Login: xcoufa09\n");
}
