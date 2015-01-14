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


void PrintFancy(map<string, int> in)
{
	wprintf(L"              string | number of occurences\n---------------------+---------------------\n");
	for (auto it = in.begin(); it != in.end(); it++)
	{
		wprintf(L"%20hs | %3i\n", it->first.c_str(), it->second);
	}
	wprintf(L"\n");
}


int main(int argc, char** argv)
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	string inString = GetFileContent("chrM.fa");

	StringFinder* sf = new StringFinderBruteForce();
	auto result = sf->GetAllSubStrings("CAGGAGGATTA", 1, 2);

	PrintFancy(result);


	wprintf(L"\n\nPress Enter...");
	wcin.ignore();
	return 0;
}
