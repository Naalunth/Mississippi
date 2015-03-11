#include "StringFinderSuffixTree.h"
#include <stack>
#include <assert.h>
#include <cmath>



SuffixTreeNode::SuffixTreeNode() : 
isRoot(0),
didGoToSibling(0){}


SuffixTreeNode::~SuffixTreeNode()
{
	if (sibling) delete sibling;
	if (child) delete child;
}

int SuffixTreeNode::EdgeLength(SuffixTree* tree) {
	return ((end == END_MARKER) ? (tree->text->length() - 1) : (end + 1)) - begin;
}


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
		/*
		wprintf(L"%sBegin:%i End:%s\n%s[%s]\n%sLeaves below:%i  Prefix length:%i\n\n",
			ind.data(), begin, end == END_MARKER ? L"#" : to_wstring(end).data(),
			ind.data(), tree->text->substr(begin, ((end == END_MARKER) ? endpos : end) - begin + 1).data(),
			ind.data(), leafnumber, labellength);
		/*/
		wprintf(L"%sBegin:%i End:%s\n\n",
			ind.data(), begin, end == END_MARKER ? L"#" : to_wstring(end).data());
		//*/
		if (child) child->Draw(indent + 1, tree, endpos);
		if (sibling) sibling->Draw(indent, tree, endpos);
	}
	else
	{
		if (child) child->Draw(indent, tree, endpos);
	}
}


SuffixTree::SuffixTree(string* in)
{
	text = in;
	BuildTree();
}


SuffixTree::~SuffixTree()
{
	if (text) delete text;
	if (root) delete root;
}



//Giant monolithic function for building a suffix tree
// Based on "On-line construction of suffix trees" (E. Ukkonen, 1995)
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
	int i;

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
				//text->at(currentLetter + ofs + 1),
				text->at(i - activePoint.length + (activeNodePointer->end - activeNodePointer->begin) + 1),
				activePoint.length - (activeNodePointer->end - activeNodePointer->begin + 1)
			};
		}
	};

	auto ResetActivePoint = [&](){
		activePoint = (activePoint.node == root) ?
			ActivePoint{ 
				root,
				text->at((activePoint.length > 0) ? ((currentLetter + 1 < text->length()) ? currentLetter + 1 : 0) : '\0' ),
				activePoint.length > 0 ? activePoint.length - 1 : 0 }
			: 
			ActivePoint{ 
				activePoint.node->suffixlink ? activePoint.node->suffixlink : root,
				activePoint.edge,
				activePoint.length };
		WalkDown();
	};

	auto AddSuffixLink = [&](SuffixTreeNode* linkTo){
		if (lastInsertedNode)
			lastInsertedNode->suffixlink = linkTo;
		lastInsertedNode = linkTo;
	};

	for (i = 0; i < text->length(); i++)
	{
		if (!(i % (text->length() / 1000))) wprintf(L"At step %.*i (%3.0f%%)\n", (int) log10((float) text->length()) + 1, i, (float) i / (float) text->length()*100.f);
		//if (!(i % (text->length() / 1000))) wprintf(L"At step %i\n", i);
		remainder++;
		for (currentLetter = i - remainder + 1; currentLetter <= i; currentLetter++)
		{
			//wprintf(L"\nStep:%i current letter:%i(%c)\n", i, currentLetter, text->at(currentLetter));
			if (activePoint.length == 0)
				activePoint.edge = text->at(i);
			activeNodePointer = activePoint.node->GetChild(activePoint.edge, this);
			assert(activePoint.node->end != END_MARKER);
			if (activeNodePointer && (text->at(activeNodePointer->begin + activePoint.length) == text->at(i)))
			//if the current character matches the text
			{
				//just change the active point and skip to the next step
				activePoint.edge = text->at(activeNodePointer->begin);
				activePoint.length++;

				AddSuffixLink(activePoint.node);

				WalkDown();
				break;
			}
			else
			{
				if (activePoint.length == 0)
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
		activeNodePointer = 0;
	}
	//root->Draw(1, this);
	fflush(stdout);
}


map<string, int> SuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	map<string, int> res;
	if (!root) return res;
	stack<SuffixTreeNode*> traversalStack;
	stack<int> leafNumberStack;
	traversalStack.push(root);
	leafNumberStack.push(0);
	

	int leafCountReturn = 0;
	int labelAccumulator = 0;
	string prefixAccumulator = "";
	SuffixTreeNode* currentNode = traversalStack.top();


newNodeInsertion:
	labelAccumulator = labelAccumulator + currentNode->EdgeLength(this);
	if (!currentNode->isRoot) prefixAccumulator.append(text->substr(currentNode->begin, currentNode->EdgeLength(this)));
	currentNode->didGoToSibling = 0;
	if (currentNode->child)
	{
		traversalStack.push(currentNode->child);
		currentNode = currentNode->child;
		goto newNodeInsertion;
	}
	else
		leafCountReturn = 1;

returnFromSomething:
returnFromChild:
	if (currentNode->didGoToSibling) { goto returnFromSibling; }

	leafNumberStack.push(leafCountReturn);

	//add this node to the result if it suffices the parameters
	if (leafCountReturn >= minAmount && labelAccumulator >= minLength)
		res[prefixAccumulator] = leafCountReturn;

	labelAccumulator -= currentNode->EdgeLength(this);
	if (!currentNode->isRoot) prefixAccumulator.resize(prefixAccumulator.size() - currentNode->EdgeLength(this));
	currentNode->didGoToSibling = 1;
	if (currentNode->sibling)
	{
		traversalStack.push(currentNode->sibling);
		currentNode = currentNode->sibling;
		goto newNodeInsertion;
	}
	else
		leafCountReturn = 0;

returnFromSibling:
	traversalStack.pop();
	if (traversalStack.empty()) goto endTraversal;
	leafCountReturn += leafNumberStack.top();
	leafNumberStack.pop();
	currentNode = traversalStack.top();
	goto returnFromSomething;



endTraversal:
	return res;
}


StringFinderSuffixTree::StringFinderSuffixTree()
{
}


StringFinderSuffixTree::~StringFinderSuffixTree()
{
}



map<string, int> StringFinderSuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	return suffixTree_->GetAllSubStrings(minLength, minAmount);
}


void StringFinderSuffixTree::OnStringChange(string* in)
{
	if (suffixTree_) delete suffixTree_;
	suffixTree_ = new SuffixTree(in);
}