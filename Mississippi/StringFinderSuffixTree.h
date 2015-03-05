#pragma once
#include "StringFinder.h"

//Suffix Tree Node
struct SuffixTreeNode
{
public:
    SuffixTreeNode* child = 0;
    SuffixTreeNode* sibling = 0;
    int begin;
    int end;
    SuffixTreeNode* suffixtreelink = 0;
    int leafoffset;

    int labellength;
    int leafnumber;

    ~SuffixTreeNode();

    //Calculates labellength and leafnumber for every node
    int CalculateLimitations(int len);
};


struct SuffixTree
{
    SuffixTreeNode* root = 0;
    std::string* text;

    SuffixTree(std::string*);
    ~SuffixTree();

    //Calculates labellength and leafnumber for every node
    void CalculateLimitations();

    //Extracts all Substrings matching the specifications
    map<string, int> GetAllSubStrings(int minLength, int minAmount);
};



class StringFinderSuffixTree :
    public StringFinder
{
public:
    StringFinderSuffixTree();
    ~StringFinderSuffixTree();
private:
    SuffixTree* suffixTree_;
};
