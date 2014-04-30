/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  22.3.2014 22:14:54
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c.1 - priklad B
 * =====================================================================
 */

/*
 * =====================================================================
 *             Modul pro zpracovani obrazku ve formatu PPM
 * =====================================================================
 */

#include "ppm.h"

/*   Nacteni obrazku
 * ---------------------------------------------------------------------
 * - nacita ze souboru do struktury ppm, alokuje ji
 * - kontroluje format obrazku
 * - v pripade chyby formatu vola funkci Warning z modulu error.c
 */
struct ppm * ppm_read(const char * filename)
{
	char head[3];
	struct ppm * pic = NULL;
	FILE * picFile = NULL;
	unsigned tmpX = 0;
	unsigned tmpY = 0;
	unsigned color = 0;

	if ((picFile = fopen(filename, "rb")) == NULL)
	{
		FatalError("Nelze otevrit soubor %s\n", filename);
	}

	if ((fscanf(picFile,"%2s %u %u %u%*c", head, &tmpX, &tmpY, &color) != 4 ) ||
		 (strcmp(head, "P6") != EXIT_SUCCESS) ||
		 ( color != 255))
	{
		Warning("Tento obrazek neni platnym obrazkem typu PPM.\n");
		fclose(picFile);
		return NULL;
	}

	const unsigned long dataSize = 3 * tmpX * tmpY * sizeof(char);

	if (dataSize >= MAX_PIC_SIZE)
	{
		fclose(picFile);
		Warning("Obrazek je vetsi nez %lu.\n", MAX_PIC_SIZE);
		return NULL;
	}

	if ((pic = (struct ppm *) malloc(sizeof(struct ppm) + dataSize)) == NULL)
	{
		fclose(picFile);
		Warning("Nepodarilo se alokovat dostatek pameti.\n");
		return NULL;
	}

	pic->xsize = tmpX;
	pic->ysize = tmpY;
	if (fread(&pic->data, sizeof(char), dataSize, picFile) != dataSize || fgetc(picFile) != EOF)
	{
		free(pic);
		fclose(picFile);
		Warning("Tento obrazek neni platnym obrazkem typu PPM.\n");
		return NULL;
	}

	fclose(picFile);
	return pic;
}

/*   Vytvoreni obrazku
 * ---------------------------------------------------------------------
 * - vytvori obrazek ze struktury PPM
 * - v pripade chyby formatu vola funkci Warning z modulu error.c
 */
int ppm_write(struct ppm *p, const char * filename)
{
	FILE * picFile = NULL;

	if ((picFile = fopen(filename, "w")) == NULL)
	{
		Warning("Soubor %s se nepodarilo otevrit pro cteni.\n", filename);
		return EXIT_FILEFAIL;
	}

	if (fprintf(picFile, "P6\n%u %u\n255\n", p->xsize, p->ysize) < 0 || fwrite(p->data, sizeof(char), 3*p->xsize*p->ysize, picFile) != 3*p->xsize*p->ysize*sizeof(char))
	{
		Warning("Zapis do souboru %s selhal.\n", filename);
	}

	fclose(picFile);
	return EXIT_SUCCESS;
}
