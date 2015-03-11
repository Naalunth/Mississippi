#pragma once
#include "StringFinder.h"



static const int END_MARKER = INT_MAX;

struct SuffixTree;

typedef unsigned char uint8;

//Suffix Tree Node
struct SuffixTreeNode
{
public:
	int begin = 0;
	int end = 0;
	uint8 isRoot : 1, didGoToSibling : 1;
	SuffixTreeNode* child = 0;
	SuffixTreeNode* sibling = 0;
	SuffixTreeNode* suffixlink = 0;

	SuffixTreeNode();
	~SuffixTreeNode();

	int EdgeLength(SuffixTree*);

	void AddChild(SuffixTreeNode*);
	void ReplaceChild(char label, SuffixTreeNode*, SuffixTree*);

	SuffixTreeNode* GetChild(char label, SuffixTree* parentTree);

	void Draw(int indent, SuffixTree*, int endpos = END_MARKER);
};


struct SuffixTree
{
	SuffixTreeNode* root = 0;
	std::string* text = 0;

	SuffixTree(std::string*);
	~SuffixTree();

	//Calculates labellength and leafnumber for every node
	void CalculateLimitations();

	//Extracts all Substrings matching the specifications
	map<string, int> GetAllSubStrings(int minLength, int minAmount);

private:
	void BuildTree();
};



class StringFinderSuffixTree :
	public StringFinder
{
public:
	StringFinderSuffixTree();
	~StringFinderSuffixTree();
	map<string, int> GetAllSubStrings(int l = 1, int k = 2);
protected:
	void OnStringChange(string* in);
private:
	SuffixTree* suffixTree_ = 0;
};
