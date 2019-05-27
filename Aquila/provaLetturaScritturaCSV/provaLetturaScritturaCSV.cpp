// provaLetturaScritturaCSV.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		cout << "nessun file .txt in input e/o in output";
		return 1;
	}
	ifstream fileInput(argv[1]);
	ofstream fileOutput(argv[2]);
	char ch;
	bool primaRiga = true;
	string s, tot = "";
	while (!fileInput.eof())
	{
		fileInput.get(ch);
		if (ch != '\n')
		{
			s = ch;
			tot.append(s);
		}
		else
		{
			if (primaRiga) {
				fileOutput << tot << ";";
				primaRiga = false;
			}
			else
				fileOutput << tot.erase(0, 64) << ";";
			tot = "";
		}
	}
	if (!tot.empty()) {
		int size = tot.size()-1;
		fileOutput << tot.erase(size,size-1).erase(0, 64) << endl;
	}
	fileInput.close();
	fileOutput.close();
	return 0;
}