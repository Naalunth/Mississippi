#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include "StringFinder.h"
#include "StringFinderBruteForce.h"
#include "StringFinderSuffixTree.h"

using namespace std;

typedef unsigned int  uint;



wstring GetFileContent(const wchar_t* filename)
{
	wstring contents;
	wifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		wprintf(L"Wasn't able to open %hs, i'm sorry. :(\n", filename);
		return wstring(filename);
	}
	file.seekg(0, ios::end);
	contents.resize(file.tellg());
	file.seekg(0, ios::beg);
	file.read(&contents[0], contents.size());
	file.close();
	return(contents);
}


void PrintFancy(map<wstring, int> in)
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
		wstring filename = L"";
		getline(wcin, filename);
		if (filename == L"")
		{
			filename = L"chrM.fa";
		}
		wstring* inString = new wstring(GetFileContent(filename.data()));

		StringFinder* sf = new StringFinderSuffixTree();
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
