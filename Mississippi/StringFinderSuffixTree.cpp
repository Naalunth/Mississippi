#include "StringFinderSuffixTree.h"
#include <stack>
#include <assert.h>





SuffixTreeNode::~SuffixTreeNode()
{
	if (sibling) delete sibling;
	if (child) delete child;
}

/*
int SuffixTreeNode::CalculateLimitations(int len, SuffixTree* tree)
{
	labellength = ((isRoot) ? (0) : (len + ((end == END_MARKER) ? (tree->text->length() - 1) : end) - begin + 1));
	leafnumber = (child ? (child->CalculateLimitations(labellength, tree)) : 1);
	return (child ? (child->CalculateLimitations(labellength, tree)) : 1)
		+ (sibling ? (sibling->CalculateLimitations(len, tree)) : 0);
}
*/


void SuffixTreeNode::AddChild(SuffixTreeNode* newChild)
{
	if (child ? 0 : child = newChild) return;
	SuffixTreeNode* ptr = child;
	while (ptr->sibling) ptr = ptr->sibling;
	ptr->sibling = newChild;
}


SuffixTreeNode* SuffixTreeNode::GetChild(char label, SuffixTree* parentTree)
{
	if (!child) return 0;
	SuffixTreeNode* ptr = child;
	for (;;)
	{
		if (parentTree->text->operator[](ptr->begin) == label) return ptr;
		if (!(ptr = ptr->sibling)) return 0;
	}
}

void SuffixTreeNode::ReplaceChild(char label, SuffixTreeNode* newchild, SuffixTree* parentTree)
{
	if (child ? 0 : child = newchild) return;
	SuffixTreeNode** parentptr = &child;
	while (parentTree->text->at((*parentptr)->begin) != label) if (!*(parentptr = &(*parentptr)->sibling)) return;
	newchild->sibling = (*parentptr)->sibling;
	*parentptr = newchild;
}

void SuffixTreeNode::Draw(int indent, SuffixTree* tree, int endpos)
{
	if (!isRoot)
	{
		wstring ind;
		int x = indent;
		while (x--) ind.append(L"    ");
		wprintf(L"%sBegin:%i End:%s\n%s[%s]\n%sLeaves below:%i  Prefix length:%i\n\n",
			ind.data(), begin, end == END_MARKER ? L"#" : to_wstring(end).data(),
			ind.data(), tree->text->substr(begin, ((end == END_MARKER) ? endpos : end) - begin + 1).data(),
			ind.data(), leafnumber, labellength);
		if (child) child->Draw(indent + 1, tree, endpos);
		if (sibling) sibling->Draw(indent, tree, endpos);
	}
	else
	{
		if (child) child->Draw(indent, tree, endpos);
	}
}


SuffixTree::SuffixTree(wstring& in)
{
	text = new wstring();
	text->reserve(in.length() + 1);
	text->assign(in);
	text->append(L"$");
	BuildTree();
}


SuffixTree::~SuffixTree()
{
	if (text) delete text;
	if (root) delete root;
}



//Giant monolithic function for building a suffix tree
// Based on "On-line construction of suffix trees" (Ukkonen, 1995)
void SuffixTree::BuildTree()
{
	struct ActivePoint
	{ 
		SuffixTreeNode* node;	//Node where the edge originates
		char edge;	//First char of the label of the edge to point at
		int length;	//Offset down the label of the edge
	} 
	activePoint;	//Pointer storing where to insert a new edge
	int remainder = 0;	//Counts the remaining inserts to the tree
	SuffixTreeNode* lastInsertedNode = 0;	//For building suffixs links
	SuffixTreeNode* activeNodePointer = 0;	//Internal helper pointer

	root = new SuffixTreeNode();
	root->isRoot = true;
	activePoint = { root, 0, 0 };
	activePoint.node = root;

	int currentLetter;

	auto WalkDown = [&](){
		//Walk down the tree in case the new point's label is too short
		int ofs = 0;		//tracks the offset from the current letter
		for (;;)
		{
			activeNodePointer = activePoint.node->GetChild(activePoint.edge, this);
			if (!activeNodePointer) break;
			if (activeNodePointer->end - activeNodePointer->begin >= activePoint.length) break;
			ofs += activeNodePointer->end - activeNodePointer->begin + 1;
			activePoint = {
				activeNodePointer,
				text->at(currentLetter + ofs + 1),
				activePoint.length - (activeNodePointer->end - activeNodePointer->begin + 1)
			};
		}
	};

	auto ResetActivePoint = [&](){
		activePoint = (activePoint.node == root) ? ActivePoint{ root, text->at(currentLetter + 1 < text->length() ? currentLetter + 1 : 0), activePoint.length - 1 >= 0 ? activePoint.length - 1 : 0 }
		: ActivePoint{ activePoint.node->suffixlink ? activePoint.node->suffixlink : root, activePoint.edge, activePoint.length };
		WalkDown();
	};

	auto AddSuffixLink = [&](SuffixTreeNode* linkTo){
		if (lastInsertedNode && lastInsertedNode != linkTo)
			lastInsertedNode->suffixlink = linkTo;
		if ((!linkTo->isRoot) && linkTo->child)
			lastInsertedNode = linkTo;
	};

	for (int i = 0; i < text->length(); i++)
	{
		remainder++;
		for (currentLetter = i - remainder + 1; currentLetter <= i; currentLetter++)
		{
			wprintf(L"\nStep:%i current letter:%i(%c)\n", i, currentLetter, text->at(currentLetter));
			activeNodePointer = activePoint.node->GetChild(activePoint.edge, this);
			assert(activePoint.node->end != END_MARKER);
			if (activeNodePointer && (text->at(activeNodePointer->begin + activePoint.length) == text->at(i)))
			//if the current character matches the text
			{
				//just change the active point and skip to the next step
				activePoint.edge = text->at(currentLetter);
				activePoint.length++;

				AddSuffixLink(activePoint.node);

				WalkDown();
				break;
			}
			else
			{
				if (!activeNodePointer || activePoint.length == 0)
				{

					//insert new child
					SuffixTreeNode* nc = new SuffixTreeNode();
					nc->begin = i;
					nc->end = END_MARKER;
					activePoint.node->AddChild(nc);

					AddSuffixLink(activePoint.node);
				}
				else
				{
					//Edge split
					assert(activeNodePointer);
					SuffixTreeNode *newinternal = new SuffixTreeNode(), *newchild = new SuffixTreeNode();
					newinternal->begin = activeNodePointer->begin;
					newinternal->end = activeNodePointer->begin + activePoint.length - 1;
					newchild->begin = i;
					newchild->end = END_MARKER;

					activePoint.node->ReplaceChild(text->at(newinternal->begin), newinternal, this);
					newinternal->child = activeNodePointer;
					activeNodePointer->sibling = newchild;

					activeNodePointer->begin = activeNodePointer->begin + activePoint.length;

					AddSuffixLink(newinternal);
				}

				ResetActivePoint();
				remainder--;
			}
			//root->Draw(1, this, i);
		}
		lastInsertedNode = 0;
	}
	CalculateLimitations();
	root->Draw(1, this);
}


void SuffixTree::CalculateLimitations()
{
	//if (root) root->CalculateLimitations(0, this);
}


map<wstring, int> SuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	if (!root) return map<wstring, int>();
	map<wstring, int> res;
	SuffixTreeNode* ptr;
	stack<SuffixTreeNode*> dftStack;
	wstring accumulatedSubstring = L"";
	dftStack.push(root);
	while (!dftStack.empty())
	{
		ptr = dftStack.top();
		dftStack.pop();

		if (ptr->sibling) dftStack.push(ptr->sibling);
		if (ptr->child) dftStack.push(ptr->child);
	}
	return res;
}


StringFinderSuffixTree::StringFinderSuffixTree()
{
}


StringFinderSuffixTree::~StringFinderSuffixTree()
{
}



map<wstring, int> StringFinderSuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	return suffixTree_->GetAllSubStrings(minLength, minAmount);
}


void StringFinderSuffixTree::OnStringChange(wstring* in)
{
	if (suffixTree_) delete suffixTree_;
	suffixTree_ = new SuffixTree(*in);
}