#pragma once
#include <map>
#include <string>
using namespace std;

class StringFinder
{
public:
	//Change the text associated with this StringFinder
	void SetString(wstring* inputSequence)
	{
		text_ = inputSequence;
		OnStringChange(inputSequence);
	}

	//Returns all Substrings matching the specifications
	virtual map<wstring, int> GetAllSubStrings(int minLength = 1, int minAmount = 2) = 0;
	virtual ~StringFinder() {};

protected:
	virtual void OnStringChange(wstring* in) = 0;
	wstring* text_;
};
