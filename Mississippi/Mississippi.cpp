#include <Windows.h>
#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include "StringFinder.h"
#include "StringFinderBruteForce.h"
#include "StringFinderSuffixTree.h"
#include "Utility.h"

using namespace std;







int GetFileContentWithTerminal(const wchar_t* filename, string** out)
{
	string* contents;
	ifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		wprintf(L"Wasn't able to open %s, i'm sorry. :(\n", filename);
		return 1;
	}
	file.seekg(0, ios::end);
	contents = new string;
	contents->resize((int) file.tellg() + 1);
	file.seekg(0, ios::beg);
	file.read(&(*contents)[0], contents->size() - 1);
	file.close();
	(*contents)[contents->size() - 1] = '$';
	*out = contents;
	return 0;
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


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	_setmode(_fileno(stdout), _O_U16TEXT);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);

	wstring inputbuffer = L"";
	map<string, int> tmpres;

	string* textInput;

	for (;;)
	{
		restart:
		wprintf(L"1: Load file\n2: Enter string\n");
		getline(wcin, inputbuffer);
		wprintf(L"\n");
		switch (stoi(inputbuffer))
		{
		case 1:
			wprintf(L"Enter filename: ");
			getline(wcin, inputbuffer);
			if (GetFileContentWithTerminal(inputbuffer.data(), &textInput))
			{
				wprintf(L"Could not load file %s\n\n", inputbuffer);
				goto restart;
			}
			break;
		case 2:
			wprintf(L"Enter string: ");
			getline(wcin, inputbuffer);
			textInput->assign(Util::wstrtostr(inputbuffer));
			textInput->append("$");
			break;
		default:
			wprintf(L"1 or 2. Just 1 or 2.\n\n");
			goto restart;
		}

		StringFinder* sf = new StringFinderSuffixTree();
		sf->SetString(textInput);

		wprintf(L"\n\n");


		int l;
		int k;
		for (;;)
		{
			string_loaded_menu:
			wprintf(L"1: Find repeated substring\n2: End\n");
			getline(wcin, inputbuffer);
			wprintf(L"\n");
			switch (stoi(inputbuffer))
			{
			case 1:
				wprintf(L"Minimum length: ");
				getline(wcin, inputbuffer);
				l = stoi(inputbuffer);
				wprintf(L"Minimum amount: ");
				getline(wcin, inputbuffer);
				k = stoi(inputbuffer);
				tmpres = sf->GetAllSubStrings(l, k);
				PrintFancy(tmpres);
				break;
			case 2:
				delete sf;
				if (textInput) delete textInput;
				goto restart;
				break;
			default:
				wprintf(L"Nope.\n");
				goto string_loaded_menu;
			}
		}

	}


	wprintf(L"\n\nPress any key...");
	wcin.ignore();
	return 0;
}
