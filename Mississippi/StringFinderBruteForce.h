#pragma once
#include "StringFinder.h"
class StringFinderBruteForce :
	public StringFinder
{
public:
	StringFinderBruteForce();
	~StringFinderBruteForce();
	map<wstring, int> GetAllSubStrings(int l = 1, int k = 2);
};

