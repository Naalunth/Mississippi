#include "StringFinderSuffixTree.h"


StringFinderSuffixTree::StringFinderSuffixTree()
{
}


StringFinderSuffixTree::~StringFinderSuffixTree()
{
}



SuffixTreeNode::~SuffixTreeNode()
{
    if (sibling) delete sibling;
    if (child) delete child;
}


int SuffixTreeNode::CalculateLimitations(int len)
{
    labellength = len + end - begin + 1;
    if (!child) return 1;
    return child->CalculateLimitations(labellength) + sibling ? sibling->CalculateLimitations(labellength) : 0;
}



SuffixTree::SuffixTree(string* in)
{
    text = in;
}


SuffixTree::~SuffixTree()
{
    delete text;
    if (root) delete root;
}


void SuffixTree::CalculateLimitations()
{
    if (root) root->CalculateLimitations(0);
}


map<string, int> GetAllSubStrings(int minLength, int minAmount)
{

}
