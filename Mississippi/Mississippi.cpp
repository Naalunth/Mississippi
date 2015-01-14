#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include "StringFinder.h"
#include "StringFinderBruteForce.h"

using namespace std;

typedef unsigned int  uint;



string GetFileContent(char* filename)
{
	string contents;
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, ios::end);
	contents.resize(file.tellg());
	file.seekg(0, ios::beg);
	file.read(&contents[0], contents.size());
	file.close();
	return(contents);
}



int main(int argc, char** argv)
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	string inString = GetFileContent("chrM.fa");

	StringFinder* sf = new StringFinderBruteForce();
	auto result = sf->GetAllSubStrings(inString, 2, 2);

	wprintf(L"string \t\tnumber of occurences\n");

	for (auto it = result.begin(); it != result.end(); it++)
	{
		wprintf(L"%s \t\t%i\n", it->first, it->second);
	}


	wprintf(L"\n\nPress Enter...");
	wcin.ignore();
	return 0;
}
