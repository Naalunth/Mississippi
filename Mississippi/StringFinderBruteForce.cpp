#include "StringFinderBruteForce.h"
#include <map>

using namespace std;

StringFinderBruteForce::StringFinderBruteForce()
{
}


StringFinderBruteForce::~StringFinderBruteForce()
{
}


map<string, int> StringFinderBruteForce::GetAllSubStrings(int l, int k)
{
	unsigned int len = text_->length();
	map<string, int> result;
	for (int i = len - 1; i >= l; i--)
	{

		map<string, int> countOfSubstring;
		for (int j = 0; j < len - i + 1; j++)
		{
			string buffer;
			buffer = text_->substr(j, i);
			if (countOfSubstring.end() == countOfSubstring.find(buffer))
			{
				countOfSubstring[buffer] = 0;
			}
			countOfSubstring[buffer]++;
		}

		for (auto it1 = countOfSubstring.begin(); it1 != countOfSubstring.end(); it1++)
		{
			if (it1->second < k)
			{
				continue;
			}
			bool bAddString = true;
			for (auto it2 = result.begin(); it2 != result.end(); it2++)
			{
				if ((it2->first.find(it1->first) != string::npos) && (it2->second >= it1->second))
				{
					bAddString = false;
				}
			}
			if (bAddString)
			{
				result.insert(*it1);
			}
		}

	}
	return result;
}