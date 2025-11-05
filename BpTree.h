#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"

class BpTree {
private:
	BpTreeNode* root;
	int	order;		// m children
	ofstream* flog;

public:
	BpTree(int order = 3) 
	{
		root = NULL;
		this->order = order;
		//flog = fout;
	}
	
	~BpTree() {

	}

	/* essential */
	bool		Insert(EmployeeData* newData);
	bool		excessDataNode(BpTreeNode* pDataNode);
	bool		excessIndexNode(BpTreeNode* pIndexNode);
	void		splitDataNode(BpTreeNode* pDataNode);
	void		splitIndexNode(BpTreeNode* pIndexNode);
	BpTreeNode* getRoot() { return root; }
	BpTreeNode* searchDataNode(string name);
	bool		searchRange(string start, string end, ofstream& flog);


	void		PrintAllData(ofstream& flog);	//B+트리의 모든 데이터 출력
};

#endif
