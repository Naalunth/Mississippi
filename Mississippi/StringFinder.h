#pragma once
#include <map>
#include <string>
using namespace std;

class StringFinder
{
public:
	//Change the text associated with this StringFinder
	void SetString(string* inputSequence)
	{
		text_ = inputSequence;
		OnStringChange(inputSequence);
	}

	//Returns all Substrings matching the specifications
	virtual map<string, int> GetAllSubStrings(int minLength = 1, int minAmount = 2) = 0;
	virtual ~StringFinder() {};

protected:
	virtual void OnStringChange(string* in) = 0;
	string* text_;
};
