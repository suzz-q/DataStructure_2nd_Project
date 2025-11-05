#include "Manager.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void Manager::run(const char* command) {
	fin.open(command);
	flog.open("log.txt");

	if (!fin)
	{
		flog << "Fail to open command file\n";
		exit(-1);
	}
	if (fin.peek() == ifstream::traits_type::eof()) // exit if command.txt is empty
		return;

	string line;
	while (getline(fin, line)) {
		if (line.empty())
			continue;

		istringstream ss(line);
		string cmd;
		ss >> cmd;  // 명령어만 분리

		if (cmd == "LOAD"){
			LOAD();
		}
		else if (cmd == "ADD_BP") {
			string name;
			int dept_no, ID, income;

			ss >> name >> dept_no >> ID>>income;

			//인자가 부족한 경우
			if (ss.fail())
			{
				printErrorCode(200);
				continue;
			}
			
				ADD_BP(name, dept_no, ID, income);
				flog << " ========ADD_BP=======\n";
				flog << name << "/" 
					<< dept_no << "/" 
					<< ID << "/" 
					<< income << "\n";
				flog << " =======================\n\n";
			
		}
		else if (cmd=="SEARCH_BP")
		{
			
			//인자를 입력 x || B+ 트리에 데이터가 없는 경우 || 검색하는 이름의 사원이 없는 경우
			string parm[3];
			ss >> parm[0];
			if (parm[0].empty())
			{
				printErrorCode(300);
			}

			ss >> parm[1];

			//이름 기준 검색
			if (parm[1].empty())
			{
				SEARCH_BP_NAME(parm[0]);
			}
			else if (!SEARCH_BP_RANGE(parm[0], parm[1]))
			{
				printErrorCode(300);
			}
			
		}
		else if (cmd== "ADD_ST") {
			string type, value;
			ss >> type >> value;
			if (type == "dept_no")
			{
				int dept_no = stoi(value);
				ADD_ST_DEPTNO(dept_no);
			}
			else if (type == "name")
			{
				ADD_ST_NAME(value);
			}
			else
			{
				printErrorCode(500);
			}
		}
		else if (cmd=="PRINT_ST") {
			int dept_no;

			ss >> dept_no;

			if (!stree)
			{
				printErrorCode(600);
			}

			if (!stree->printEmployeeData(dept_no))
			{
				printErrorCode(600);
			}
			
			
		}
		else if (cmd == "PRINT_BP") {
			if (bptree->getRoot() == nullptr) //저장된 데이터가 없는 경우
			{
				printErrorCode(400);
			}
			else 
			{
				bptree->PrintAllData(flog);
			}
		}
		else if (cmd == "DELETE") {
			if (!stree || !stree->Delete())
				printErrorCode(700);
			else
				printSuccessCode("DELETE");
		}
		else if (cmd == "EXIT") {
			
			printSuccessCode("EXIT");
			return;
		}
		else // invalid command
			printErrorCode(800);
	}
}

void Manager::LOAD() {
	ifstream fdata;
	fdata.open("employee.txt");
	if (fdata.is_open() && bptree->getRoot() == nullptr)
	{
		if (fdata.peek() == ifstream::traits_type::eof()) { // If the first character of the file is eof, the file is empty
			printErrorCode(100);
			return;
		}

		string empName;
		int dept_no,	//부서 코드
			ID,			//사번
			annual_income;	//da
		while (fdata >> empName >> dept_no >> ID >> annual_income)
		{
			EmployeeData* empData = new EmployeeData; //creat a new object
			empData->setName(empName);
			empData->setDeptNo(dept_no);
			empData->setID(ID);
			empData->setIncome(annual_income);
			bptree->Insert(empData);
		}
		printSuccessCode("LOAD");
	}
	else
	{
		printErrorCode(100);
	}
	

}

void Manager::ADD_BP(string name, int dept_no, int ID, int income) 
{
	BpTreeNode* foundNode = bptree->searchDataNode(name);
	bool exists = false;


	if (foundNode) {
		for (auto& data : *foundNode->getDataMap()) {
			if (data.first == name) {
				exists = true;
				break;
			}
		}
	}

	if (exists) {
		printErrorCode(200); // 중복
		return;
	}
	
	EmployeeData* empData = new EmployeeData;
	empData->setName(name);
	empData->setDeptNo(dept_no);
	empData->setID(ID);
	empData->setIncome(income);
	bptree->Insert(empData);
}

void Manager::SEARCH_BP_NAME(string name) {
	if (!bptree || !bptree->getRoot()) {
		printErrorCode(300);
		return;
	}

	BpTreeNode* node = bptree->searchDataNode(name);
	if (!node) {
		printErrorCode(300);
		return;
	}
	bool found = false;
	for (auto& iter : *node->getDataMap()) {
		if (iter.first == name) {
			flog << "========SEARCH_BP========\n";
			flog << iter.second->getName() << "/"
				<< iter.second->getDeptNo() << "/"
				<< iter.second->getID() << "/"
				<< iter.second->getIncome() << "\n";
			flog << "=======================\n\n";
			found = true;
			break;
		}
	}

	if (!found)
		printErrorCode(300);
}

bool Manager::SEARCH_BP_RANGE(string start, string end) 
{
	if (bptree->getRoot() == nullptr)
	{
		return false;
	}
	return bptree->searchRange(start, end, flog);
	

}

void Manager::ADD_ST_DEPTNO(int dept_no) 
{
	if(!bptree || !bptree->getRoot()) 
	{
		printErrorCode(500);
		return;
	}

	// Selection Tree 생성
	if (!stree) {
		stree = new SelectionTree(&flog);
	}

	stree->setTree();


	//왼쪽부터 탐색
	BpTreeNode* node = bptree->getRoot();

	while (node && node->getMostLeftChild()) node = node->getMostLeftChild(); // leaf 도달

	bool inserted = false;

	while (node) {
		for (auto& kv : *node->getDataMap()) {
			EmployeeData* emp = kv.second;
			if (emp->getDeptNo() == dept_no) {
				stree->Insert(emp);
				inserted = true;
			}
		}
		node = node->getNext(); // 다음 leaf 노드로
	}
	if (!inserted)
		printErrorCode(500); // 해당 부서 직원 없음
	else
		printSuccessCode("ADD_ST");

}

void Manager::ADD_ST_NAME(string name) 
{
	//B+ 트리가 비었는지 확인
	if (!bptree || !bptree->getRoot()) {
		printErrorCode(500);
		return;
	}

	BpTreeNode* addNode = bptree->searchDataNode(name);
	if (!addNode) { // 해당 이름의 노드가 없는 경우
		printErrorCode(500);
		return;
	}

	//이름이 일치하는 사원 찾기 
	EmployeeData* foundData = nullptr;
	for (auto& kv : *addNode->getDataMap()) {
		if (kv.second->getName() == name) {
			foundData = kv.second;
			break;
		}
	}

	if (!foundData) {
		printErrorCode(500); // 이름 존재하지 않음
		return;
	}

	//Selection Tree 초기화 
	if (!stree) {
		stree = new SelectionTree(&flog);
		stree->setTree();
	}

	//Selection Tree에 삽입
	if (!stree->Insert(foundData)) {
		printErrorCode(500);
		return;
	}

	printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP() 
{
	if (bptree->getRoot() == nullptr)	//B+트리에 저장된 데이터가 없는 경우
	{
		printErrorCode(400);
		return;
	}

	//bptree->searchRange("a", "z"); 
}

void Manager::PRINT_ST() {
	int dept_no;

	// 파일에서 부서 번호 읽기
	if (!(fin >> dept_no)) {
		printErrorCode(600); // 인자 없음
		return;
	}

	// Selection Tree가 생성되어 있는지 확인
	if (!stree) {
		printErrorCode(600);
		return;
	}

	// 실제 출력
	if (!stree->printEmployeeData(dept_no)) {
		printErrorCode(600); // 해당 부서 없음
	}
	else {
		printSuccessCode("PRINT_ST");
	}
}

void Manager::DELETE() {
	if (!stree) {
		printErrorCode(700);
		return;
	}

	// 삭제 시도
	if (!stree->Delete()) {
		printErrorCode(700); // Tree가 비었거나 삭제 불가
	}
	else {
		printSuccessCode("DELETE");
	}
}

void Manager::printErrorCode(int n) {
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success) {
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}