/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  08.12.2013 23:58:09
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c. 3
 *         Soubor:  proj3.c
 * =====================================================================
 */

/*
 * =====================================================================
 *    Program pro hledani obrazcu v bitmape
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*   Struktura pro bitmapu
 * ---------------------------------------------------------------------
 */
typedef struct {
  int rows;
  int cols;
  char *cells;
} Bitmap;

/*   Prototypy vytvorenych fukci
 * ---------------------------------------------------------------------
 */
int parseArgv(int argc, char * argv[], bool * modeHelp, bool * modeTest, bool * modeHline, bool * modeVline, bool * modeSquare, char ** fileName);
int loadMap(Bitmap * bitmap, bool test, char * fileName);
int allocMap(Bitmap * bitmap, int rows, int cols);
void unAllocMap(Bitmap * bitmap);
int isSquare(Bitmap * bitmap, int x1, int y1, int x2, int y2);
/* Podukoly */
char getcolor(Bitmap *bitmap, int x, int y);
int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);

void printErr(char * msg);
void printHelp();

/*   Hlavni program
 * ---------------------------------------------------------------------
 * - necha zpracovat argumenty pro dany mod a nasledne zavola
 *   pozadovanou funkci
 * - necha alokovat strukturu Bitmap a pak ji i necha uvolnit
 * - pohlida spravny prubeh a vypise vysledky
 */
int hf = 0;

int main(int argc, char * argv[])
{
  char * fileName = "";
  Bitmap bitmap;
  bool found;
  bool modeTest = false;
  bool modeHline = false;
  bool modeVline = false;
  bool modeSquare = false;
  bool modeHelp = false;
  int x1, x2, y1, y2;

  
  if (parseArgv(argc, argv, &modeHelp, &modeTest, &modeHline, &modeVline, &modeSquare, &fileName) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  if (modeHelp)
  {
    printHelp();
    return EXIT_SUCCESS;
  }
    
  if (loadMap(&bitmap, modeTest, fileName) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (modeTest)
    return EXIT_SUCCESS;
  else if (modeHline)
  {
    found = find_hline(&bitmap, &x1, &y1, &x2, &y2);
  }
  else if (modeVline)
  {
    found = find_vline(&bitmap, &x1, &y1, &x2, &y2);
  }
  else if (modeSquare)
  {
    found = find_square(&bitmap, &x1, &y1, &x2, &y2);
  } else
  {
    printErr("Nebyla specifikovana volba");
    return EXIT_FAILURE;
  }

  unAllocMap(&bitmap);
  
  if (found != 0)
    printf("%d %d %d %d\n", y1, x1, y2, x2);
  else
    printf("Obrazec nebyl nalezen\n");

  printf("%d", hf);
  return EXIT_SUCCESS;
}

/*   Nejdelsi svisla cara
 * ---------------------------------------------------------------------
 * - prochazi polem a z kazdeho jejiho bodu hleda nejdelsi svislou caru
 * - pokud se v matici dostane do mist, kde uz delsi caru nebude moci
 *   nalezt, nehleda dal
 * - vraci delku nejdelsi cary
 * - upravi hodnoty argumentu predavanych odkazem, tak aby odkazovali na
 *   zacatek a konec nejdelsi cary
 */
int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int x1tmp, x2tmp, y1tmp, y2tmp, maxNew;
  int max = 0;
  
  for (int i = 0; i < (*bitmap).rows - max; i++)
  {
    for (int j = 0; j < (*bitmap).cols; j++)
    {
      if (getcolor(bitmap, j, i) == 1)
      {
        x1tmp = j;
        y1tmp = i;
        for (int k = i; k < (*bitmap).rows; k++)
        {
          if (getcolor(bitmap, j, k) == 1)
          {
            x2tmp = j;
            y2tmp = k;
          } else
          {
            break;
          }
        }
        if (max < (maxNew = y2tmp - y1tmp))
        {
          max = maxNew;
          *x1 = x1tmp;
          *x2 = x2tmp;
          *y1 = y1tmp;
          *y2 = y2tmp;
        }
      }
    }
  }
  return max;
}

/*   Nejdelsi vodorovna cara
 * ---------------------------------------------------------------------
 * - prochazi polem a z kazdeho jejiho bodu hleda nejdelsi vodorovna caru
 * - pokud se v matici dostane do mist, kde uz delsi caru nebude moci
 *   nalezt, nehleda dal
 * - vraci delku nejdelsi cary
 * - upravi hodnoty argumentu predavanych odkazem, tak aby odkazovali na
 *   zacatek a konec nejdelsi cary
 */
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int x1tmp, x2tmp, y1tmp, y2tmp, maxNew;
  int max = 0;
  
  for (int i = 0; i < (*bitmap).rows; i++)
  {
    for (int j = 0; j < (*bitmap).cols - max; j++)
    {
      if (getcolor(bitmap, j, i) == 1)
      {
        x1tmp = j;
        y1tmp = i;
        for (int k = j; k < (*bitmap).cols; k++)
        {
          if (getcolor(bitmap, k, i) == 1)
          {
            x2tmp = k;
            y2tmp = i;
          } else
          {
            break;
          }
        }
        if (max < (maxNew = x2tmp - x1tmp))
        {
          max = maxNew;
          *x1 = x1tmp;
          *x2 = x2tmp;
          *y1 = y1tmp;
          *y2 = y2tmp;
        }
      }
    }
  }
  return max;
}

/*   Nejvetsi ctverec
 * ---------------------------------------------------------------------
 * - prochazi polem a z kazdeho jejiho bodu (povazuje jej za levy horni
 *   vrchol) hleda ctverec
 * - pokud se v matici dostane do mist, kde uz vetsi ctverec nebude moci
 *   nalezt, nehleda dal
 * - vraci delku strany ctverce
 * - upravi hodnoty argumentu predavanych odkazem, tak aby odkazovali na
 *   levy horni a pravy dolni vrchol ctverce
 */
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int x1tmp, x2tmp, y1tmp, y2tmp, maxNew;
  int max = 0;
  int current;

  for (int i = 0; i < (*bitmap).rows - max; i++)
  {
    current = getcolor(bitmap, 0, i);
    printf("%d", current);
    for (int j = 0; j < (*bitmap).cols - max; j++)
    {
      if (current && (current = getcolor(bitmap, j + 1, i)) && getcolor(bitmap, j, i + 1))
      {
        x1tmp = j;
        y1tmp = i;
        for (int k = max; (k+i < (*bitmap).rows && k+j < (*bitmap).cols); k++)
        {
          if (getcolor(bitmap, x1tmp+k, y1tmp+k) == 1)
          {
            x2tmp = x1tmp+k;
            y2tmp = y1tmp+k;
            if (max < (maxNew = x2tmp - x1tmp) && isSquare(bitmap, x1tmp, y1tmp, x2tmp, y2tmp) == EXIT_SUCCESS)
            {
              max = maxNew;
              *x1 = x1tmp;
              *x2 = x2tmp;
              *y1 = y1tmp;
              *y2 = y2tmp;
            }
          }
        }
      }
    }
  }
  return max;
}

/*   Overeni ctverce
 * ---------------------------------------------------------------------
 * - funkce overi, jestli je mezi danymi koncovymi body ctverec
 * - proje strany ctverce a vrati, jestli jsou neporusene
 */
int isSquare(Bitmap * bitmap, int x1, int y1, int x2, int y2)
{
  bool square = true;
  for (int i = x1 + 1; i < x2; i++)
  {
    if ((getcolor(bitmap, i, y1) != 1) || (getcolor(bitmap, i, y2) != 1))
    {
      square = false;
    }
  }
  for (int i = y1 + 1; i < y2; i++)
  {
    if ((getcolor(bitmap, x1, i) != 1) || (getcolor(bitmap, x2, i) != 1))
    {
      square = false;
    }
  }
  if ((getcolor(bitmap, x1, y2) != 1) || (getcolor(bitmap, x2, y1) != 1))
  {
    square = false;
  }

  return square ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*  Nacteni bitmapy do pameti a test souboru
 * ---------------------------------------------------------------------
 * - nacte bitmapu ze souboru a ulozi ji do struktury Bitmap
 * - necha pro Bitmap alokovat pamet
 * - pokud je pozadovan pouze test souboru data neukada, jen kontroluje
 *   a rozhodne o spravnosti souboru
 */
int loadMap(Bitmap * bitmap, bool test, char * fileName)
{
  int i = 0;
  int c, alloc;
  int rows = 0;
  int cols = 0;
  bool valid = true;
  FILE * source = NULL;

/* otevereni souboru */
  source  = fopen(fileName, "r");
  if (source == NULL)
  {
    printErr("Soubor nenalezen.");
    return EXIT_FAILURE;
  }

/* nacteni poctu radku a sloupcu */
  if (fscanf(source, "%d", &rows) != 1)
  {
    valid = false;
  }

  if (fscanf(source, "%d", &cols) != 1)
  {
    valid = false;
  }
  
/* nacteni obsahu matice */
  if (!test)
  { 
    alloc = allocMap(bitmap, rows, cols);
    if (alloc != EXIT_SUCCESS)
    {
      return EXIT_FAILURE;
    }
  }
  while (fscanf(source, "%d", &c) == 1)
  {
    if (c != 0 && c != 1)
    {
        valid = false;
        break;
    }
    if (!test)
    {
      *((*bitmap).cells + i) = (char)c;
    }
    i++;
  }

/* pokud nesedi pocet pixelu s poctem radku a sloupcu */
  if (i != rows * cols)
  {
      valid = false;
  }

  fclose(source);

  if (valid)
  {
    if (test)
    {
      printf("Valid\n");
    }
    return EXIT_SUCCESS;
  } else
  {
    if (test)
    {
      printf("Invalid\n");
      return EXIT_SUCCESS;
    } else
    {
      printErr("Bitmapu se nezdarilo nacist do pameti");
      return EXIT_FAILURE;
    }
  }
}

/*   Alokace struktury
 * ---------------------------------------------------------------------
 * - alokuje pamet pro strukturu Bitmap podle zadanych parametru
 */
int allocMap(Bitmap * bitmap, int rows, int cols)
{
  (*bitmap).rows = rows;
  (*bitmap).cols = cols;
  (*bitmap).cells = malloc(rows * cols * sizeof(char));
  if ((*bitmap).cells == NULL)
  {
    return EXIT_FAILURE;
  } else
  {
    return EXIT_SUCCESS;
  }
}

/*   Uvolneni pameti
 * ---------------------------------------------------------------------
 * - uvolneni pameti alokovane strukturou Bitmap a nastaveni ostatnich
 *   parametru na 0
 */
void unAllocMap(Bitmap * bitmap)
{
  free((*bitmap).cells);
  (*bitmap).rows = 0;
  (*bitmap).cols = 0;
  (*bitmap).cells = NULL;
}

/*  Hodnota prvku
 * ---------------------------------------------------------------------
 * - zjisti hodnotu (barvu) prvku v matici a vrati ji
 */
char getcolor(Bitmap *bitmap, int x, int y)
{
  hf++;
  return *((*bitmap).cells + (y * (*bitmap).cols) + x);
}

/*  Zpracovani argumentu
 * ---------------------------------------------------------------------
 * - projde jednotlive argumenty a zpracuje je
 * - pohlida pripadne chyby
 */
int parseArgv(int argc, char * argv[], bool * modeHelp, bool * modeTest, bool * modeHline, bool * modeVline, bool * modeSquare, char ** fileName)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == EXIT_SUCCESS || strcmp(argv[i], "-h") == EXIT_SUCCESS)
    {
      *modeHelp = true;
      return EXIT_SUCCESS;
    }
  }

  if (argc != 3)
  {
    printErr("Nespravny pocet agrumentu.");
    return EXIT_FAILURE;
  }
  
  *fileName = argv[2];
  
  if (strcmp(argv[1], "--test") == EXIT_SUCCESS)
  {
    *modeTest = true;
  }
  else if (strcmp(argv[1], "--hline") == EXIT_SUCCESS)
  {
    *modeHline = true;
  }
  else if (strcmp(argv[1], "--vline") == EXIT_SUCCESS)
  {
    *modeVline = true;
  }
  else if (strcmp(argv[1], "--square") == EXIT_SUCCESS)
  {
    *modeSquare = true;
  }
  else
  {
    printErr("Neznama volba.");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
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
  printf("Pro ziskani napovedy pouzijte: ./proj3 --help\n");
}
void printHelp()
{
  printf("Program pro hledani nejdelsich car ci nejvetsich ctvercu v bitmape\n");
  printf("Spousteni: ./proj3 VOLBA SOUBOR\n\n");
  printf("Volby:\n");
  printf("  --hline                    nalezne nejdelsi horizontalni usecku\n");
  printf("  --vline                    nalezne nejdelsi vertikalni usecku\n");
  printf("  --square                   naleyne nejvetsi ctverec\n");
  printf("  -h  --help                 vypise tuto napovedu a skonci\n\n");
  printf("Autor: Tomas Coufal  <xcoufa09@stud.fit.vutbr.cz>\n");
  printf("Login: xcoufa09\n");
}
