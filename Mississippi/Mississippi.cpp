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



string GetFileContent(const char* filename)
{
	string contents;
	ifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		wprintf(L"Wasn't able to open %hs, i'm sorry. :(\n", filename);
		return 0;
	}
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

	for (;;)
	{
		wprintf(L"Enter filename: ");
		string filename = "";
		getline(cin, filename);
		if (filename == "")
		{
			filename = "chrM.fa";
		}
		string* inString = new string(GetFileContent(filename.c_str()));

		StringFinder* sf = new StringFinderBruteForce();
        sf->SetString(inString);
        auto result1 = sf->GetAllSubStrings(6, 2);
        auto result2 = sf->GetAllSubStrings(10, 2);

        PrintFancy(result1);
        PrintFancy(result2);
	}


	wprintf(L"\n\nPress Enter...");
	wcin.ignore();
	return 0;
}
