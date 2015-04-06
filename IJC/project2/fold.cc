/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/25/2014 05:44:34 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 1
 * =====================================================================
 */

/*
 * =====================================================================
 *                    Volna implementace fold v C++
 * =====================================================================
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define DEFAULT_LENGTH 80

using namespace std;

struct paramsS {
	int lineLength = DEFAULT_LENGTH;
	bool cflag = false;
	string inputFile = "";
};

int fold(paramsS * a);
void printErr(string msg);

/*   Main
 * ---------------------------------------------------------------------
 * - zpracovava argumenty vola funkci fold
 */
int main(int argc, char * argv [])
{
	cout.sync_with_stdio(false);
	cin.sync_with_stdio(false);

	paramsS param;
	if (argc > 1 && argc <= 5)
	{
		for (int i = 1; i < argc; i++)
		{
			if (string(argv[i]) == "-c") { param.cflag = true; }
			else if (string(argv[i]) == "-w" )
			{
				string s = argv[++i];
				istringstream is(s);
				is >> param.lineLength;
			} else if (i < argc - 1)
			{
				printErr("Unrecognized option.");
			} else
			{
				param.inputFile = argv[i];
			}
		}
	}

	fold(&param);
	return EXIT_SUCCESS;
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
int fold(paramsS * a)
{
	istream *input = &cin;
	fstream fileIn;

	fileIn.open(a->inputFile, ios::in);
	if (!fileIn.is_open())
		printErr("Unable to open file.");
	else
		input = &fileIn;

	string line, word;
	int lineCounter = 0;
	bool wasEmpty = false;
	bool errout = false;
	while (getline(*input, line))
	{

		if (line.empty()) { cout << endl; wasEmpty = true; lineCounter = 0; continue; }

		if (wasEmpty) { wasEmpty = false; cout << endl; }
		istringstream lineStream(line);
		while (lineStream >> word)
		{
			if ((lineCounter + int(word.length())) > a->lineLength)
			{
				cout << endl;
				lineCounter = 0;
			} else if (lineCounter != 0)
			{
				cout << " ";
				lineCounter++;
			}
			if (int(word.length()) >= a->lineLength)
			{
				if (!errout) { printErr("Word too long, cutting."); errout = true; }
				if (a->cflag) {  word.erase(a->lineLength);}
			}
			cout << word;
			lineCounter += word.length();
		}
	}
	cout << endl;

	fileIn.close();
	return EXIT_SUCCESS;
}

void printErr(string msg)
{
	cerr << msg << endl;
	return;
}
