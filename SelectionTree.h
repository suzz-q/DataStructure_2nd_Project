#pragma once
#include "SelectionTreeNode.h"

class SelectionTree {
private:
    SelectionTreeNode* root;
    ofstream* fout;
    SelectionTreeNode* run[8];

public:
    SelectionTree(ofstream* fout) 
    {
        this->fout = fout;
        root = nullptr;
        for (int i = 0; i < 8; i++)
        {
            run[i] = nullptr;
        }
    }

    ~SelectionTree() {

    }

    void setRoot(SelectionTreeNode* pN) { this->root = pN; }
    SelectionTreeNode* getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);
};