#include "BpTree.h"

bool BpTree::Insert(EmployeeData* newData) {
	if (root == nullptr) //트리가 비었을 때 
	{
		BpTreeDataNode* node = new BpTreeDataNode;
		node->insertDataMap(newData->getName(), newData);
		root = node;
		return true;
	}
	

	BpTreeNode* node = root;
	if (!node) return false;
	
	while (node && !node->is_DataNode()) {
		//bool moved = false;

		auto iter = node->getIndexMap()->end();
		iter--;

		while (true) {
			if (newData->getName().compare(iter->first) >= 0) {
				// 해당 key보다 크거나 같으면 오른쪽 자식으로
				if (iter->second) {
					node = iter->second;
				}
				else {
					node = nullptr;
				}
				break;
			}
			if (iter == node->getIndexMap()->begin()) {
				node = node->getMostLeftChild();
				break;
			}
			iter--;
		}
	}


	//iter map으로 선언할건지 auto로 선언할건지 통일해야할듯
	//같은 데이터가 존재하는지 확인
	for (auto iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++)
	{

		if (iter->first.compare(newData->getName()) == 0)
		{
			//income만 업데이트한다고 봤던 것 같은데 진짜 뭐지 왜 없지 
			//income value update
			iter->second->setIncome(newData->getIncome());
			delete newData;
			return true;
		}

		
	}

	//중복 되지 않는 경우 데이터 ㅅ바입
	node->insertDataMap(newData->getName(), newData);

	//데이터 개수가 초과한 경우 (default:3)
	if (excessDataNode(node)) //노드가 3개인 경우 
	{
		//트리에 인덱스 노드가 없는 경우
		if (node->getParent() == NULL)
		{
			BpTreeIndexNode* newNode = new BpTreeIndexNode;
			node->setParent(newNode);
			newNode->setMostLeftChild(node);
			root = newNode;
			splitDataNode(node);

			return true;
		}

		//루트가 아닌 데이터 노드인 경우
		splitDataNode(node);

		//split한 후 부모 노드도 넘쳤는지 검사
		while (1)
		{
			//대상 노드를 부모 노드로 변경
			node = node->getParent();

			if (excessIndexNode(node))
			{
				if (node->getParent() == NULL)
				{
					BpTreeIndexNode* newNode = new BpTreeIndexNode;
					node->setParent(newNode);

					newNode->setMostLeftChild(node);
					root = newNode;
					splitIndexNode(node);
					return true;
				}
				//더이상 분할이 필요 없을 때까지 위로 올라감
				splitIndexNode(node);
			}
			else //분할 끝
			{
				return true;
			}
		}
	}

	return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode) 
{
	if (pDataNode->getDataMap()->size() < this->order) {
		return false;
	}
	return true;

}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) 
{
	if (pIndexNode->getIndexMap()->size() < this->order) {
		return false;
	}
	return true;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) 
{
	BpTreeDataNode* newDataNode = new BpTreeDataNode;

	//위치 계산
	int splitLocation = ceil((order - 1) / 2.0);
	auto iter = pDataNode->getDataMap()->begin();
	advance(iter, splitLocation);


	for (auto it = iter; it != pDataNode->getDataMap()->end();)
	{
		newDataNode->insertDataMap(it->first, it->second);
		it = pDataNode->getDataMap()->erase(it);
	}
	
	//업데이트 
	newDataNode->setNext(pDataNode->getNext());
	if (pDataNode->getNext())
	{
		pDataNode->getNext()->setPrev(newDataNode);
	}

	pDataNode->setNext(newDataNode);                //set pDataNode's next
	newDataNode->setPrev(pDataNode);                //set new Node's prev

	
	newDataNode->setParent(pDataNode->getParent());     //set new Node's parent

	if (pDataNode->getParent() == nullptr) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode;
		newRoot->setMostLeftChild(pDataNode);
		pDataNode->setParent(newRoot);
		root = newRoot;
	}

	BpTreeIndexNode* parent = static_cast<BpTreeIndexNode*>(pDataNode->getParent());
	
	parent->insertIndexMap(newDataNode->getDataMap()->begin()->first, newDataNode);
	newDataNode->setParent(parent);

}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) 
{
	
	int splitLocation = ceil((order - 1) / 2.0);
	auto iter = pIndexNode->getIndexMap()->begin();
	advance(iter, splitLocation - 1); //중간 이전 키

	string propKey = next(iter)->first; //부모로 올라갈 key
	
	//새로운 인덱스 노드
	BpTreeIndexNode* newIndexchildNode = new BpTreeIndexNode;
	newIndexchildNode->setParent(pIndexNode->getParent());

	//데이터 이동 전에 MostLeftChild 설정
	newIndexchildNode->setMostLeftChild(next(iter)->second);
	newIndexchildNode->getMostLeftChild()->setParent(newIndexchildNode);


	auto sKey = next(iter, 2);	//오른쪽으로 옮길 key
	//오른쪽 데이터 이동
	while (sKey != pIndexNode->getIndexMap()->end())
	{
		newIndexchildNode->insertIndexMap(sKey->first, sKey->second);
		sKey = pIndexNode->getIndexMap()->erase(sKey);
	}

	pIndexNode->getIndexMap()->erase(next(iter));  // propKey 제거

	
	if (pIndexNode->getParent() == nullptr)
	{
		BpTreeIndexNode* newRoot = new BpTreeIndexNode;
		pIndexNode->setParent(newRoot);
		newRoot->setMostLeftChild(pIndexNode);
		root = newRoot;
	}
	
	//newIndexchildNode->setParent(pIndexNode->getParent());
	pIndexNode->getParent()->insertIndexMap(propKey, newIndexchildNode);

	//newIndexChildNode의 자식 포인터 Parent 업데이트
	for (auto& it : *newIndexchildNode->getIndexMap()) {
		it.second->setParent(newIndexchildNode);
	}
}

BpTreeNode* BpTree::searchDataNode(string name) 
{
	if (root == NULL)
	{
		return NULL;
	}

	BpTreeNode* node = root;

	while (node->getMostLeftChild())
	{
		auto iter = node->getIndexMap()->end();
		iter--;

		while (1)
		{
			if ((iter->first).compare(name) <= 0)
			{
				node = iter->second;
				break;
			}
			else if (iter == node->getIndexMap()->begin())
			{
				node = node->getMostLeftChild();
				break;
			}
			else
			{
				iter--;
			}
		}
	}
	return node;
}

bool BpTree::searchRange(string start, string end, ofstream& flog)
{

	BpTreeNode* node = searchDataNode(start);
	if (!node)
		return false;

	if (node->getPrev() && node->getDataMap()->begin()->first > start)
		node = node->getPrev();


	if (node->getDataMap()->begin()->first > end)
	{
		return false;
	}
	bool found = false;
	bool stop = false;
	

	while (node)
	{
		map <string, EmployeeData*>::iterator iter;
		for (iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++)
		{
			string first;
			first += iter->first[0];
			if (iter->first>=start && iter->first<=end)
			{
				if(!found) //발견된 적 없음== 첫번째
				{
					flog << "========SEARCH_BP========\n";
					found = true;
				}
				flog << iter->second->getName() << "/" 
					<< iter->second->getDeptNo() << "/" 
					<< iter->second->getID() << "/" 
					<< iter->second->getIncome() << "\n";
				
			}
			else if (iter->first > end)
			{
				stop = true;
				break;
			}
		}

		if (!stop)
		{
			node = node->getNext();
		}
		else
			break;
		
	}
	if (found)
	{
		flog << "=======================\n\n";
	}

	return found;
}

void BpTree::PrintAllData(ofstream& flog)
{
	//In-Order 방식으로 출력 (오름차순)
	
	if (root == NULL) //데이터가 없는 경우 바로 끝냄
	{
		return;
	}

	BpTreeNode* node = root;

	//가장 왼쪽 노드로 이동
	while (node->getMostLeftChild())
	{
		node = node->getMostLeftChild();
	}

	map <string, EmployeeData*>::iterator iter;
	flog << "========PRINT_BP========\n";
	while (node)
	{
		
		for (iter = node->getDataMap()->begin(); iter != node->getDataMap()->end(); iter++)
		{
			flog << iter->second->getName() << "/"
				<< iter->second->getDeptNo() << "/"
				<< iter->second->getID() << "/"
				<< iter->second->getIncome() << "\n";
		}

		node = node->getNext();
	}
	flog << " =======================\n\n";
	
	
}