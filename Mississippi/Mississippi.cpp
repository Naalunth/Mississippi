#include <Windows.h>
#include <stdio.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>
#include <cctype>
#include <cassert>

#include <list>
#include <vector>
#include <algorithm>
#include <list>

#include "StringFinder.h"
#include "StringFinderBruteForce.h"
#include "StringFinderSuffixTree.h"
#include "Utility.h"

using namespace std;






int GetFileContent(const wchar_t* filename, string** out)
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
	contents->resize((int) file.tellg());
	file.seekg(0, ios::beg);
	file.read(&(*contents)[0], contents->size() - 1);
	file.close();
	*out = contents;
	return 0;
}


int GetFileContentWithTerminal(const wchar_t* filename, string** out)
{
	string* contents;
	ifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		wprintf(L"Wasn't able to open %s, i'm sorry. :(\n", filename);
		return 1;
	}
	wprintf(L"Loading %s...\n", filename);
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


int ParseFasta(const wchar_t* filename, string** out, bool doFilter = false)
{
	if (GetFileContentWithTerminal(filename, out)) return 1;
	wprintf(L"Parsing file...\n");
	wprintf(L"\tRemoving headers, comments...\n");
	for (;;)
	{
		auto first = (*out)->begin();
		if (*first == '>' || *first == ';')
		{
			auto last = first;
			while (*last != '\r' && *last != '\n') last++;
			if (*(last + 1) == '\n') last++;
			(*out)->erase(first, last + 1);
		}
		else
			break;
	}
	wprintf(L"\tRemoving unnecessary stuff...\n");
	(*out)->resize(distance((*out)->begin(), remove_if((*out)->begin(), (*out)->end(), isspace)));
	if (doFilter) (*out)->resize(distance((*out)->begin(), remove_if((*out)->begin(), (*out)->end(), [](char c)->bool{return !(c == 'A' || c == 'C' || c == 'G' || c == 'T'); })));
	transform((*out)->begin(), (*out)->end(), (*out)->begin(), toupper);
	return 0;
}


void PrintFancy(const map<PosLen, vector<int> >& in, string* text)
{
	if (in.size() == 0)
	{
		wprintf(L"No substrings found.\n\n");
		return;
	}
	if (in.size() <= 25)
	{
		wprintf(L"The numbers show the amount of substrings starting at that position in the text.\n\n");
		for (auto it = in.begin(); it != in.end(); it++)
		{
			wprintf(L"%hs (%i times)\n", text->substr(it->first.pos, it->first.len).data(), it->second.size());
			map<int, int> tmppos;
			for (auto it0 = it->second.begin(); it0 != it->second.end(); it0++)
				tmppos[*it0 * 100 / text->size()]++;
			string s(100, '_');
			for (auto it0 = tmppos.begin(); it0 != tmppos.end(); it0++)
				s[it0->first] = (it0->second < 10) ? (((char) it0->second) + (char) 0x30) : ('*');
			wprintf(L"|%s|\n\n", (Util::strtowstr(s).data()));
		}
	}
	else
	{
		wprintf(L"              string | number of occurences\n---------------------+---------------------\n");
		for (auto it = in.begin(); it != in.end(); it++)
		{
			wprintf(L"%20hs | %3i\n", text->substr(it->first.pos, it->first.len).data(), it->second.size() == 999 ? 999 : it->second.size());
		}
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



	map<PosLen, vector<int> > tmpres;
	wstring inputbuffer = L"";

	string* textInput;

	wprintf(L"Repeated substring finder.\nby Kevin Schier\n(No warranty whatsoever for crashes on invalid inputs.)\n\n");

	for (;;)
	{
		restart:
		wprintf(L"1: Load file\n2: Load FASTA file\n3: Load FASTA file (only ACGT)\n4: Enter string\n9: Bye!\n");
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
			wprintf(L"Enter filename: ");
			getline(wcin, inputbuffer);
			if (ParseFasta(inputbuffer.data(), &textInput))
			{
				wprintf(L"Could not load file %s\n\n", inputbuffer);
				goto restart;
			}
			break;
		case 3:
			wprintf(L"Enter filename: ");
			getline(wcin, inputbuffer);
			if (ParseFasta(inputbuffer.data(), &textInput, true))
			{
				wprintf(L"Could not load file %s\n\n", inputbuffer);
				goto restart;
			}
			break;
		case 4:
			wprintf(L"Enter string: ");
			getline(wcin, inputbuffer);
			textInput = new string;
			textInput->assign(Util::wstrtostr(inputbuffer));
			textInput->append("$");
			break;
		case 9:
			goto end_session;
		default:
			wprintf(L"Nope.\n\n");
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
				wprintf(L"\n");
				PrintFancy(tmpres, textInput);
				tmpres = map<PosLen, vector<int>>();
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

	end_session:

	wprintf(L"\nBye!");

	return 0;
}
