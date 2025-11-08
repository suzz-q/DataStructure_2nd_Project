#include "SelectionTree.h"
#include <cmath>

void SelectionTree::setTree()
{
    // run = 8 ¡æ 8 departments (100~800)
    // Create a full binary tree with level = 3

    for (int i = 0; i < 8; i++)
    {
        run[i] = new SelectionTreeNode;
        run[i]->HeapInit(); // Initialize EmployeeHeap for each leaf (department)
    }

    // Second level (level2): create 4 internal nodes to group leaves
    SelectionTreeNode* level2[4];
    SelectionTreeNode* level1[2];

    // Connect leaf (8 nodes) ¡æ level2 (4 nodes)
    for (int i = 0; i < 4; i++)
    {
        level2[i] = new SelectionTreeNode;
        level2[i]->setLeftChild(run[i * 2]);
        level2[i]->setRightChild(run[i * 2 + 1]);
        run[i * 2]->setParent(level2[i]);
        run[i * 2 + 1]->setParent(level2[i]);
    }

    // Connect level2 (4 nodes) ¡æ level1 (2 nodes)
    for (int i = 0; i < 2; i++)
    {
        level1[i] = new SelectionTreeNode;
        level1[i]->setLeftChild(level2[i * 2]);
        level1[i]->setRightChild(level2[i * 2 + 1]);
        level2[i * 2]->setParent(level1[i]);
        level2[i * 2 + 1]->setParent(level1[i]);
    }

    // Connect the root node (level1 has two children)
    root = new SelectionTreeNode;
    root->setLeftChild(level1[0]);
    root->setRightChild(level1[1]);
    level1[0]->setParent(root);
    level1[1]->setParent(root);
}

bool SelectionTree::Insert(EmployeeData* newData)
{
    if (!newData)
        return false;

    // Convert department number (100~800) to index (0~7)
    int index = (newData->getDeptNo() / 100) - 1;
    if (index < 0 || index >= 8)
        return false;

    SelectionTreeNode* leaf = run[index];
    if (!leaf->getHeap())
        leaf->HeapInit(); // Initialize department heap (if necessary)

    // Insert employee into the department heap
    leaf->getHeap()->Insert(newData);
    if (leaf->getHeap())
        leaf->setEmployeeData(leaf->getHeap()->Top()); // Store the highest-paid employee in the node

    // Move upward toward the parent and update the Selection Tree by comparing nodes
    SelectionTreeNode* node = leaf;
    while (node->getParent())
    {
        SelectionTreeNode* parent = node->getParent();
        SelectionTreeNode* left = parent->getLeftChild();
        SelectionTreeNode* right = parent->getRightChild();

        EmployeeData* leftData = (left) ? left->getEmployeeData() : nullptr;
        EmployeeData* rightData = (right) ? right->getEmployeeData() : nullptr;

        // Set the higher-paid employee among two children as the parent node data
        if (leftData == nullptr && rightData == nullptr)
            parent->setEmployeeData(nullptr);
        else if (rightData == nullptr || (leftData && leftData->getIncome() >= rightData->getIncome()))
            parent->setEmployeeData(leftData);
        else
            parent->setEmployeeData(rightData);

        node = parent; // Move upward
    }

    return true;
}

bool SelectionTree::Delete() {
    // When the tree is empty or the root has no data
    if (!root || !root->getEmployeeData())
        return false;

    // Store the highest-paid employee in the root as target
    EmployeeData* target = root->getEmployeeData();
    int index = (target->getDeptNo() / 100) - 1; // Move to corresponding department

    SelectionTreeNode* leaf = run[index];
    if (!leaf->getHeap() || leaf->getHeap()->IsEmpty())
        return false;

    // Delete the root (highest-paid employee) from the heap
    leaf->getHeap()->Delete();

    // Update leaf data after deletion (new highest-paid employee)
    leaf->setEmployeeData(leaf->getHeap()->Top());

    // Reorder upward to the root to maintain SelectionTree structure
    SelectionTreeNode* node = leaf;
    while (node->getParent())
    {
        SelectionTreeNode* parent = node->getParent();
        SelectionTreeNode* left = parent->getLeftChild();
        SelectionTreeNode* right = parent->getRightChild();

        EmployeeData* leftData = (left) ? left->getEmployeeData() : nullptr;
        EmployeeData* rightData = (right) ? right->getEmployeeData() : nullptr;

        // Set the parent data to the higher-paid employee among two children
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

    EmployeeHeap* original = leaf->getHeap();

    // Keep the original heap unchanged and use a copied heap for output
    EmployeeHeap tempHeap;

    // Copy all data from the original heap (preserving income order)
    for (int i = 1; i <= original->getSize(); i++) {
        EmployeeData* data = original->getHeapArr()[i];
        if (data)
            tempHeap.Insert(data);
    }

    (*fout) << "========PRINT_ST========\n";

    // Extract one by one from the copied heap and print (descending by income)
    while (!tempHeap.IsEmpty()) {
        EmployeeData* e = tempHeap.Top();  // Highest-paid employee
        if (e) {
            (*fout) << e->getName() << "/"
                << e->getDeptNo() << "/"
                << e->getID() << "/"
                << e->getIncome() << "\n";
        }
        tempHeap.Delete(); // Remove after printing (move to next)
    }

    (*fout) << "=======================\n\n";
    return true;
}
