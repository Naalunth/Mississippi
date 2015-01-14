#pragma once
#include "StringFinder.h"
class StringFinderBruteForce :
	public StringFinder
{
public:
	StringFinderBruteForce();
	~StringFinderBruteForce();
	map<string, int> GetAllSubStrings(string inputSequence, int l = 1, int k = 2);
};

