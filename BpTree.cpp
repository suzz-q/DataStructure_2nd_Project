#include "BpTree.h"
#include <cmath>

bool BpTree::Insert(EmployeeData* newData) {
	if (root == nullptr) // If the tree is empty, create a root data node
	{
		BpTreeDataNode* node = new BpTreeDataNode;
		node->insertDataMap(newData->getName(), newData);
		root = node;
		return true;
	}

	BpTreeNode* node = root;
	if (!node) return false;

	// Traverse to the leaf (DataNode)
	while (node && !node->is_DataNode()) {
		// Traverse the index nodes to find the correct position
		auto iter = node->getIndexMap()->end();
		iter--;

		while (true) {
			// Move to the right if the current key is less than or equal to the name
			if (newData->getName().compare(iter->first) >= 0) {
				if (iter->second) node = iter->second;
				else node = nullptr;
				break;
			}
			// Move to the leftmost child if smaller than the smallest key
			if (iter == node->getIndexMap()->begin()) {
				node = node->getMostLeftChild();
				break;
			}
			iter--;
		}
	}

	// After reaching the leaf node, check for duplicate data
	for (auto iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++)
	{
		if (iter->first.compare(newData->getName()) == 0)
		{
			// If the name already exists, update only the income
			iter->second->setIncome(newData->getIncome());
			delete newData;
			return true;
		}
	}

	// Insert data if not duplicated
	node->insertDataMap(newData->getName(), newData);

	// If the data node exceeds the order, perform a split
	if (excessDataNode(node))
	{
		// If it is a root data node without a parent, create a new index node as the root
		if (node->getParent() == NULL)
		{
			BpTreeIndexNode* newNode = new BpTreeIndexNode;
			node->setParent(newNode);
			newNode->setMostLeftChild(node);
			root = newNode;
			splitDataNode(node);
			return true;
		}

		// Normal data node split
		splitDataNode(node);

		// After splitting, check if the parent index node also exceeds the limit ¡æ repeat up to the root
		while (1)
		{
			node = node->getParent();

			if (excessIndexNode(node))
			{
				// If the parent is the root, create a new root
				if (node->getParent() == NULL)
				{
					BpTreeIndexNode* newNode = new BpTreeIndexNode;
					node->setParent(newNode);
					newNode->setMostLeftChild(node);
					root = newNode;
					splitIndexNode(node);
					return true;
				}
				// If not the root, continue splitting upward
				splitIndexNode(node);
			}
			else // No more split needed
			{
				return true;
			}
		}
	}
	return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode)
{
	// Check if the data node exceeds the order
	if (pDataNode->getDataMap()->size() < this->order) {
		return false;
	}
	return true;
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode)
{
	// Check if the index node exceeds the order
	if (pIndexNode->getIndexMap()->size() < this->order) {
		return false;
	}
	return true;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode)
{
	BpTreeDataNode* newDataNode = new BpTreeDataNode;

	// Calculate the split position
	int splitLocation = ceil((order - 1) / 2.0);
	auto iter = pDataNode->getDataMap()->begin();
	advance(iter, splitLocation);

	// Move data after the split position to the new node
	for (auto it = iter; it != pDataNode->getDataMap()->end();) {
		newDataNode->insertDataMap(it->first, it->second);
		it = pDataNode->getDataMap()->erase(it);
	}

	// Update the doubly linked list connection (between leaf nodes)
	newDataNode->setNext(pDataNode->getNext());
	if (pDataNode->getNext())
		pDataNode->getNext()->setPrev(newDataNode);
	pDataNode->setNext(newDataNode);
	newDataNode->setPrev(pDataNode);

	newDataNode->setParent(pDataNode->getParent());

	// If there is no root, create a new root index node
	if (pDataNode->getParent() == nullptr) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode;
		newRoot->setMostLeftChild(pDataNode);
		pDataNode->setParent(newRoot);
		root = newRoot;
	}

	// Insert the first key of the new node into the parent index node
	BpTreeIndexNode* parent = static_cast<BpTreeIndexNode*>(pDataNode->getParent());
	parent->insertIndexMap(newDataNode->getDataMap()->begin()->first, newDataNode);
	newDataNode->setParent(parent);
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode)
{
	// Calculate the split position of the index node
	int splitLocation = ceil((order - 1) / 2.0);
	auto iter = pIndexNode->getIndexMap()->begin();
	advance(iter, splitLocation - 1); // One key before the middle

	string propKey = next(iter)->first; // Key to be promoted to the parent

	// Create a new index node (right node)
	BpTreeIndexNode* newIndexchildNode = new BpTreeIndexNode;
	newIndexchildNode->setParent(pIndexNode->getParent());

	// Set MostLeftChild of the new index node
	newIndexchildNode->setMostLeftChild(next(iter)->second);
	newIndexchildNode->getMostLeftChild()->setParent(newIndexchildNode);

	// Move data to the right of the split point
	auto sKey = next(iter, 2);
	while (sKey != pIndexNode->getIndexMap()->end()) {
		newIndexchildNode->insertIndexMap(sKey->first, sKey->second);
		sKey = pIndexNode->getIndexMap()->erase(sKey);
	}
	pIndexNode->getIndexMap()->erase(next(iter)); // Remove key promoted to parent

	// If there is no parent, create a new root
	if (pIndexNode->getParent() == nullptr) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode;
		pIndexNode->setParent(newRoot);
		newRoot->setMostLeftChild(pIndexNode);
		root = newRoot;
	}

	// Insert the new key and right node into the parent index node
	pIndexNode->getParent()->insertIndexMap(propKey, newIndexchildNode);

	// Update the parent of the child nodes in the new index node
	for (auto& it : *newIndexchildNode->getIndexMap()) {
		it.second->setParent(newIndexchildNode);
	}
}

BpTreeNode* BpTree::searchDataNode(string name)
{
	if (root == NULL)
		return NULL;

	BpTreeNode* node = root;

	// Traverse until reaching the leaf node
	while (node->getMostLeftChild()) {
		auto iter = node->getIndexMap()->end();
		iter--;

		while (1) {
			// Move left or right depending on the comparison result
			if ((iter->first).compare(name) <= 0) {
				node = iter->second;
				break;
			}
			else if (iter == node->getIndexMap()->begin()) {
				node = node->getMostLeftChild();
				break;
			}
			else {
				iter--;
			}
		}
	}
	return node;
}

bool BpTree::searchRange(string start, string end, ofstream& flog)
{
	BpTreeNode* node = searchDataNode(start);
	if (!node) return false;

	// Adjust the start position (check previous node)
	if (node->getPrev() && node->getDataMap()->begin()->first > start)
		node = node->getPrev();

	// If the minimum key of the start node is greater than end, skip search
	if (node->getDataMap()->begin()->first > end)
		return false;

	bool found = false;
	bool stop = false;

	// Traverse the leaf nodes to search for data within the range
	while (node) {
		for (auto iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++) {
			if (iter->first >= start && iter->first <= end) {
				if (!found) { // Print header when first found
					flog << "========SEARCH_BP========\n";
					found = true;
				}
				flog << iter->second->getName() << "/"
					<< iter->second->getDeptNo() << "/"
					<< iter->second->getID() << "/"
					<< iter->second->getIncome() << "\n";
			}
			else if (iter->first > end) { // Stop when exceeding the range
				stop = true;
				break;
			}
		}

		if (!stop) node = node->getNext();
		else break;
	}
	if (found) flog << "=======================\n\n";

	return found;
}

void BpTree::PrintAllData(ofstream& flog)
{
	// Print all data in In-Order from the leftmost leaf
	if (root == NULL) return;

	BpTreeNode* node = root;
	while (node->getMostLeftChild()) node = node->getMostLeftChild();

	flog << "========PRINT_BP========\n";
	while (node) {
		for (auto iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++) {
			flog << iter->second->getName() << "/"
				<< iter->second->getDeptNo() << "/"
				<< iter->second->getID() << "/"
				<< iter->second->getIncome() << "\n";
		}
		node = node->getNext();
	}
	flog << " =======================\n\n";
}
