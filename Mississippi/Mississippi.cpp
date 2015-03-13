#include <Windows.h>
#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include <list>
#include <vector>
#include <algorithm>

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


void PrintFancy(const map<string, int>& in)
{
	wprintf(L"              string | number of occurences\n---------------------+---------------------\n");
	for (auto it = in.begin(); it != in.end(); it++)
	{
		wprintf(L"%20hs | %3i\n", it->first.c_str(), it->second);
	}
	wprintf(L"\n");
}


bool CompareStringsBackwards(const string& a, const string& b)
{
	//returns a < b
	auto aIt = a.rbegin();
	auto bIt = b.rbegin();
	for (;;)
	{
		if (aIt == a.rend()) return true;
		if (bIt == b.rend()) return false;
		if ((*aIt) < (*bIt)) return true;
		if ((*aIt) > (*bIt)) return false;
		aIt++;
		bIt++;
	}
}


void FilterMaximalResults(map<string, int>& in)
{
	vector<string> substrings;
	substrings.reserve(in.size());

	for (auto it = in.begin(); it != in.end(); it++)
		substrings.push_back(it->first);

	sort(substrings.begin(), substrings.end(), CompareStringsBackwards);

	for (int i = 0; i <= substrings.size() - 2; i++)
	{
		if (in[substrings[i]] == in[substrings[i + 1]])
		{
			string a = substrings[i], b = substrings[i + 1];
			auto aIt = a.rbegin();
			auto bIt = b.rbegin();
			for (;;)
			{
				if (aIt == a.rend())
				{
					in.erase(a);
					break;
				}
				if (bIt == b.rend()) break;
				if ((*aIt) < (*bIt)) break;
				if ((*aIt) > (*bIt)) break;
				aIt++;
				bIt++;
			}
		}
	}

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	_setmode(_fileno(stdout), _O_U16TEXT);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);

	map<string, int> tmpres;
	wstring inputbuffer = L"";

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
			textInput = new string;
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
				wprintf(L"\nPlease wait...\n\n");
				tmpres = sf->GetAllSubStrings(l, k);
				FilterMaximalResults(tmpres);
				PrintFancy(tmpres);
				tmpres = map<string,int>();
				break;
			case 2:
				delete sf;
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
