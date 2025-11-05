#include "SelectionTree.h"
#include <cmath>

void SelectionTree::setTree() 
{
    //run=8이므로 level이 3인 full Binary Tree를 초기화 해야함 
    
    for (int i = 0; i < 8; i++)
    {
        run[i] = new SelectionTreeNode;
        run[i]->HeapInit(); // EmployeeHeap 생성
    }

    //트리 생성
    SelectionTreeNode* level2[4];
    SelectionTreeNode* level1[2];

    //연결
    for (int i = 0; i < 4; i++)
    {
        level2[i] = new SelectionTreeNode;
        level2[i]->setLeftChild(run[i * 2]);
        level2[i]->setRightChild(run[i * 2 + 1]);
        run[i * 2]->setParent(level2[i]);
        run[i * 2 + 1]->setParent(level2[i]);
    }

    // 중간 노드 연결
    for (int i = 0; i < 2; i++)
    {
        level1[i] = new SelectionTreeNode;
        level1[i]->setLeftChild(level2[i * 2]);
        level1[i]->setRightChild(level2[i * 2 + 1]);
        level2[i * 2]->setParent(level1[i]);
        level2[i * 2 + 1]->setParent(level1[i]);
    }

    root = new SelectionTreeNode;
    root->setLeftChild(level1[0]);
    root->setRightChild(level1[1]);
    level1[0]->setParent(root);
    level1[1]->setParent(root);
}

bool SelectionTree::Insert(EmployeeData* newData) 
{
    if (!newData)
    {
        return false;
    }

    int index = (newData->getDeptNo() / 100) - 1; // 100→0, 200→1, ...
    if (index < 0 || index >= 8)
        return false;

    SelectionTreeNode* leaf = run[index];
    if (!leaf->getHeap())
        leaf->HeapInit();

    leaf->getHeap()->Insert(newData);
    if (leaf->getHeap())
        leaf->setEmployeeData(leaf->getHeap()->Top());

    SelectionTreeNode* node = leaf;
    while (node->getParent())
    {
        SelectionTreeNode* parent = node->getParent();
        SelectionTreeNode* left = parent->getLeftChild();
        SelectionTreeNode* right = parent->getRightChild();

        EmployeeData* leftData = (left) ? left->getEmployeeData() : nullptr;
        EmployeeData* rightData = (right) ? right->getEmployeeData() : nullptr;

        if (leftData == nullptr && rightData == nullptr)
            parent->setEmployeeData(nullptr);
        else if (rightData == nullptr || (leftData && leftData->getIncome() >= rightData->getIncome()))
            parent->setEmployeeData(leftData);
        else
            parent->setEmployeeData(rightData);

        node = parent;
    }

    return true;

}

bool SelectionTree::Delete() {
    if (!root || !root->getEmployeeData())
        return false;

    EmployeeData* target = root->getEmployeeData();
    int index = (target->getDeptNo() / 100) - 1;

    SelectionTreeNode* leaf = run[index];
    if (!leaf->getHeap() || leaf->getHeap()->IsEmpty())
        return false;

    // Heap에서 root 삭제
    leaf->getHeap()->Delete();

    // leaf 데이터 갱신
    leaf->setEmployeeData(leaf->getHeap()->Top());

    // 위로 올라가며 SelectionTree 재정렬
    SelectionTreeNode* node = leaf;
    while (node->getParent())
    {
        SelectionTreeNode* parent = node->getParent();
        SelectionTreeNode* left = parent->getLeftChild();
        SelectionTreeNode* right = parent->getRightChild();

        EmployeeData* leftData = (left) ? left->getEmployeeData() : nullptr;
        EmployeeData* rightData = (right) ? right->getEmployeeData() : nullptr;

        if (leftData == nullptr && rightData == nullptr)
            parent->setEmployeeData(nullptr);
        else if (rightData == nullptr || (leftData && leftData->getIncome() >= rightData->getIncome()))
            parent->setEmployeeData(leftData);
        else
            parent->setEmployeeData(rightData);

        node = parent;
    }

    return true;
}

bool SelectionTree::printEmployeeData(int dept_no) {
    int index = (dept_no / 100) - 1;
    if (index < 0 || index >= 8)
        return false;

    SelectionTreeNode* leaf = run[index];
    if (!leaf->getHeap() || leaf->getHeap()->IsEmpty())
        return false;

    EmployeeHeap* heap = leaf->getHeap();

    (*fout) << "========PRINT_ST========\n";
    for (int i = 1; i <= heap->getSize(); i++)
    {
        EmployeeData* e = heap->getHeapArr()[i];
        if (e)
        {
            (*fout) << e->getName() << "/"
                << e->getDeptNo() << "/"
                << e->getID() << "/"
                << e->getIncome() << "\n";
        }
    }
    (*fout) << "=======================\n\n";

    return true;
}