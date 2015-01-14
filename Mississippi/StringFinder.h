#pragma once
#include <map>
#include <string>
using namespace std;

class StringFinder
{
public:
	virtual map<string, int> GetAllSubStrings(string inputSequence, int minLength = 1, int minAmount = 2) = 0;
	virtual ~StringFinder() {};
};