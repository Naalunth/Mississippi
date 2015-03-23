#include "StringFinderSuffixTree.h"
#include <stack>
#include <assert.h>
#include <cmath>
#include <vector>



SuffixTreeNode::SuffixTreeNode() : 
isRoot(0),
didGoToSibling(0),
isLeaf(1){}


SuffixTreeNodeInternal* SuffixTreeNode::thisInternal()
{
	return (isLeaf) ? 0 : ((SuffixTreeNodeInternal*)this);
}


SuffixTreeNode::~SuffixTreeNode()
{
	if (sibling) delete sibling;
	if (!isLeaf) if (thisInternal()->child) delete thisInternal()->child;
}

int SuffixTreeNode::EdgeLength(SuffixTree* tree) {
	return isLeaf ?
		(tree->text->length() - 1 - begin)
		: ((thisInternal()->end + 1) - begin);
}


void SuffixTreeNode::AddChild(SuffixTreeNode* newChild)
{
	assert(!isLeaf);
	if (thisInternal()->child ? 0 : thisInternal()->child = newChild) return;
	SuffixTreeNode* ptr = thisInternal()->child;
	while (ptr->sibling) ptr = ptr->sibling;
	ptr->sibling = newChild;
}


SuffixTreeNode* SuffixTreeNode::GetChild(char label, SuffixTree* parentTree)
{
	if (isLeaf) return 0;
	if (!thisInternal()->child) return 0;
	SuffixTreeNode* ptr = thisInternal()->child;
	for (;;)
	{
		if (parentTree->text->operator[](ptr->begin) == label) return ptr;
		if (!(ptr = ptr->sibling)) return 0;
	}
}

void SuffixTreeNode::ReplaceChild(char label, SuffixTreeNode* newchild, SuffixTree* parentTree)
{
	assert(!isLeaf);
	if (thisInternal()->child ? 0 : thisInternal()->child = newchild) return;
	SuffixTreeNode** parentptr = &thisInternal()->child;
	while (parentTree->text->at((*parentptr)->begin) != label) if (!*(parentptr = &(*parentptr)->sibling)) return;
	newchild->sibling = (*parentptr)->sibling;
	*parentptr = newchild;
}

void SuffixTreeNode::Draw(int indent, SuffixTree* tree)
{
	if (!isRoot)
	{
		wstring ind;
		int x = indent;
		while (x--) ind.append(L"    ");
		if (isLeaf)
			wprintf(L"%sLeaf Begin:%i\n\n",
			ind.data(), begin);
		else
			wprintf(L"%sBegin:%i End:%s\n\n",
			ind.data(), begin, to_wstring(thisInternal()->end).data());
		if (!isLeaf && thisInternal()->child) thisInternal()->child->Draw(indent + 1, tree);
		if (sibling) sibling->Draw(indent, tree);
	}
	else
	{
		if (thisInternal()->child) thisInternal()->child->Draw(indent, tree);
	}
}


SuffixTreeNodeInternal::SuffixTreeNodeInternal()
{
	isLeaf = 0;
}

SuffixTreeNodeLeaf::SuffixTreeNodeLeaf()
{
	isLeaf = 1;
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
	wprintf(L"Starting tree build...\n");
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

	root = new SuffixTreeNodeInternal();
	root->isRoot = true;
	activePoint = { root, 0, 0 };
	activePoint.node = root;

	int currentLetter;
	int i;

	auto WalkDown = [&](){
		//Walk down the tree in case the new point's label is too short
		int ofs = 0;		//tracks the offset from the current letter
		SuffixTreeNodeInternal* internalPointer;
		for (;;)
		{
			internalPointer = (SuffixTreeNodeInternal*) activePoint.node->GetChild(activePoint.edge, this);
			if (!internalPointer) break;
			if (internalPointer->isLeaf) break;
			if (internalPointer->end - internalPointer->begin >= activePoint.length) break;
			ofs += internalPointer->end - internalPointer->begin + 1;
			activePoint = {
				internalPointer,
				//text->at(currentLetter + ofs + 1),
				text->at(i - activePoint.length + (internalPointer->end - internalPointer->begin) + 1),
				activePoint.length - (internalPointer->end - internalPointer->begin + 1)
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
					activePoint.node->thisInternal()->suffixlink ? activePoint.node->thisInternal()->suffixlink : root,
				activePoint.edge,
				activePoint.length };
		WalkDown();
	};

	auto AddSuffixLink = [&](SuffixTreeNode* linkTo){
		if (lastInsertedNode)
			lastInsertedNode->thisInternal()->suffixlink = linkTo;
		lastInsertedNode = linkTo;
	};

	for (i = 0; i < text->length(); i++)
	{
		if (text->length() >= 100)
		{
			if (!(i % (text->length() / 100))) wprintf(L"At step %.*i (%3.0f%%)\n", (int) log10((float) text->length()) + 1, i, (float) i / (float) text->length()*100.f);
		}
		else
		{
			wprintf(L"At step %.*i (%3.0f%%)\n", (int) log10((float) text->length()) + 1, i, (float) i / (float) text->length()*100.f);
		}
		//if (!(i % (text->length() / 1000))) wprintf(L"At step %i\n", i);
		remainder++;
		for (currentLetter = i - remainder + 1; currentLetter <= i; currentLetter++)
		{
			//wprintf(L"\nStep:%i current letter:%i(%c)\n", i, currentLetter, text->at(currentLetter));
			if (activePoint.length == 0)
				activePoint.edge = text->at(i);
			activeNodePointer = activePoint.node->GetChild(activePoint.edge, this);
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
					SuffixTreeNodeLeaf* nc = new SuffixTreeNodeLeaf();
					nc->begin = i;
					nc->labelOffset = currentLetter;
					activePoint.node->AddChild(nc);

					AddSuffixLink(activePoint.node);
				}
				else
				{
					//Edge split
					assert(activeNodePointer);
					SuffixTreeNodeInternal *newinternal = new SuffixTreeNodeInternal();
					SuffixTreeNodeLeaf *newchild = new SuffixTreeNodeLeaf();
					newinternal->begin = activeNodePointer->begin;
					newinternal->end = activeNodePointer->begin + activePoint.length - 1;
					newchild->begin = i;
					newchild->labelOffset = currentLetter;

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
	wprintf(L"Tree built.\n");
	fflush(stdout);
}


map<PosLen, vector<int> > SuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	map<PosLen, vector<int> > res;
	if (!root) return res;
	stack<SuffixTreeNode*> traversalStack;
	stack<vector<int> > leafNumberStack;
	traversalStack.push(root);
	leafNumberStack.push(vector<int>());
	

	vector<int> leafCountReturn;
	int labelAccumulator = 0;
	SuffixTreeNode* currentNode = traversalStack.top();


	wprintf(L"Hopping through the tree collecting strings...\n");


newNodeInsertion:
	labelAccumulator = labelAccumulator + currentNode->EdgeLength(this);
	currentNode->didGoToSibling = 0;
	if (!currentNode->isLeaf)
	{
		traversalStack.push(currentNode->thisInternal()->child);
		currentNode = currentNode->thisInternal()->child;
		goto newNodeInsertion;
	}
	else
		leafCountReturn = vector<int>(1,((SuffixTreeNodeLeaf*)currentNode)->labelOffset);

returnFromSomething:
returnFromChild:
	if (currentNode->didGoToSibling) { goto returnFromSibling; }

	leafNumberStack.push(leafCountReturn);

	//add this node to the result if it suffices the parameters
	if (leafCountReturn.size() >= minAmount && labelAccumulator > minLength)
		res[{leafCountReturn[0], labelAccumulator - 1}] = leafCountReturn
		;

	labelAccumulator -= currentNode->EdgeLength(this);
	currentNode->didGoToSibling = 1;
	if (currentNode->sibling)
	{
		traversalStack.push(currentNode->sibling);
		currentNode = currentNode->sibling;
		goto newNodeInsertion;
	}
	else
		leafCountReturn = vector<int>();

returnFromSibling:
	traversalStack.pop();
	if (traversalStack.empty()) goto endTraversal;
	leafCountReturn.insert(leafCountReturn.end(), leafNumberStack.top().begin(), leafNumberStack.top().end());
	if (leafCountReturn.size() > 999) leafCountReturn.resize(999);
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
	if(suffixTree_) delete suffixTree_;
}



map<PosLen, vector<int> > StringFinderSuffixTree::GetAllSubStrings(int minLength, int minAmount)
{
	return suffixTree_->GetAllSubStrings(minLength, minAmount);
}


void StringFinderSuffixTree::OnStringChange(string* in)
{
	if (suffixTree_) delete suffixTree_;
	suffixTree_ = new SuffixTree(in);
}