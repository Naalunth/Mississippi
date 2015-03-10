#pragma once
#include "StringFinder.h"



static const int END_MARKER = INT_MAX;

struct SuffixTree;


//Suffix Tree Node
struct SuffixTreeNode
{
public:
	int begin = 0;
	int end = 0;
	bool isRoot = false;
	SuffixTreeNode* child = 0;
	SuffixTreeNode* sibling = 0;
	SuffixTreeNode* suffixlink = 0;
	//int leafoffset;

	int labellength = 0;
	int leafnumber = 0;


	~SuffixTreeNode();


	void AddChild(SuffixTreeNode*);
	void ReplaceChild(char label, SuffixTreeNode*, SuffixTree*);

	SuffixTreeNode* GetChild(char label, SuffixTree* parentTree);

	void Draw(int indent, SuffixTree*, int endpos = END_MARKER);
};


struct SuffixTree
{
	SuffixTreeNode* root = 0;
	std::wstring* text = 0;

	SuffixTree(std::wstring&);
	~SuffixTree();

	//Calculates labellength and leafnumber for every node
	void CalculateLimitations();

	//Extracts all Substrings matching the specifications
	map<wstring, int> GetAllSubStrings(int minLength, int minAmount);

private:
	void BuildTree();
};



class StringFinderSuffixTree :
	public StringFinder
{
public:
	StringFinderSuffixTree();
	~StringFinderSuffixTree();
	map<wstring, int> GetAllSubStrings(int l = 1, int k = 2);
protected:
	void OnStringChange(wstring* in);
private:
	SuffixTree* suffixTree_ = 0;
};
