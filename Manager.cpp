#include "Manager.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void Manager::run(const char* command) {
	fin.open(command);
	flog.open("log.txt");

	// Handle when the file cannot be opened
	if (!fin)
	{
		flog << "Fail to open command file\n";
		exit(-1);
	}

	// Handle when the file is empty
	if (fin.peek() == ifstream::traits_type::eof())
		return;

	string line;
	// Read the file line by line and split by whitespace
	while (getline(fin, line)) {
		if (line.empty())
			continue;

		istringstream ss(line);

		string cmd;
		ss >> cmd;  // Extract only the command

		if (cmd == "LOAD")
		{
			LOAD(); // Read data from employee.txt and store it in the B+ tree
		}
		else if (cmd == "ADD_BP") {
			string name;
			int dept_no, ID, income;

			ss >> name >> dept_no >> ID >> income;

			// Handle insufficient arguments
			if (ss.fail())
			{
				printErrorCode(200);
				continue;
			}

			// Store directly in the B+ tree
			ADD_BP(name, dept_no, ID, income);
			// Output to file
			flog << " ========ADD_BP=======\n";
			flog << name << "/"
				<< dept_no << "/"
				<< ID << "/"
				<< income << "\n";
			flog << " =======================\n\n";

		}
		else if (cmd == "SEARCH_BP")
		{
			// When arguments are missing || B+ tree has no data || no employee found by name
			string parm[3];

			// If the first argument is missing: print error code
			ss >> parm[0];
			if (parm[0].empty())
			{
				printErrorCode(300);
				continue;
			}

			ss >> parm[1];

			// If the second argument (parm[1]) is empty ¡æ search by name; otherwise, search by range
			// Search by name
			if (parm[1].empty())
			{
				SEARCH_BP_NAME(parm[0]);
			}
			else if (!SEARCH_BP_RANGE(parm[0], parm[1]))
			{
				printErrorCode(300);
			}

		}
		else if (cmd == "ADD_ST") {
			string type, value;
			ss >> type >> value;

			if (type.empty() || value.empty())
			{
				printErrorCode(500);
				continue;
			}

			if (type == "dept_no")
			{
				try {
					int dept_no = stoi(value);

					// Check if it is a valid department code
					if (dept_no < 100 || dept_no > 800 || dept_no % 100 != 0)
					{
						printErrorCode(500);
						continue;
					}

					ADD_ST_DEPTNO(dept_no);
				}
				catch (std::invalid_argument&) {
					// When value is not a number
					printErrorCode(500);
					continue;
				}
				catch (std::out_of_range&) {
					// When the number is too large
					printErrorCode(500);
					continue;
				}
			}
			else if (type == "name")
			{
				if (value.empty()) {
					printErrorCode(500);
					continue;
				}
				ADD_ST_NAME(value);
			}
			else
			{
				printErrorCode(500);
			}
		}
		else if (cmd == "PRINT_ST") {
			int dept_no;

			// Extract department number
			if (!(ss >> dept_no)) 
			{ 
				printErrorCode(600);
				continue;
			}

			if (!stree)
			{
				printErrorCode(600);
				continue;
			}

			if (!stree->printEmployeeData(dept_no))
			{
				printErrorCode(600);
			}

		}
		else if (cmd == "PRINT_BP") {
			PRINT_BP();
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

void Manager::LOAD()
{
	// Open employee data file
	ifstream fdata;
	fdata.open("employee.txt");
	if (fdata.is_open() && bptree->getRoot() == nullptr)
	{
		if (fdata.peek() == ifstream::traits_type::eof()) { // If the first character of the file is EOF, the file is empty
			printErrorCode(100);
			return;
		}

		string empName;
		int dept_no,	// Department code
			ID,			// Employee ID
			annual_income;	// Annual income
		while (fdata >> empName >> dept_no >> ID >> annual_income)
		{
			// Create a new object
			EmployeeData* empData = new EmployeeData;
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

	if (foundNode)
	{
		// Iterate through all employee data stored in the found B+ tree node
		for (auto& data : *foundNode->getDataMap())
		{
			if (data.first == name) {
				exists = true;
				break;
			}
		}
	}

	if (exists) {
		
		return;
	}

	// Create and insert new data
	EmployeeData* empData = new EmployeeData;
	empData->setName(name);
	empData->setDeptNo(dept_no);
	empData->setID(ID);
	empData->setIncome(income);
	bptree->Insert(empData);
}

void Manager::SEARCH_BP_NAME(string name)
{
	// If B+ tree object does not exist or contains no data
	if (!bptree || !bptree->getRoot()) {
		printErrorCode(300);
		return;
	}

	// Get search result by name
	BpTreeNode* node = bptree->searchDataNode(name);
	if (!node)
	{
		printErrorCode(300);
		return;
	}

	bool found = false;

	// Iterate through all data in the node
	for (auto& iter : *node->getDataMap())
	{
		// If match found
		if (iter.first == name)
		{
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
	// When bptree object is null or contains no data
	if (!bptree || bptree->getRoot() == nullptr)
	{
		return false;
	}
	return bptree->searchRange(start, end, flog);
}

void Manager::ADD_ST_DEPTNO(int dept_no)
{
	// When bptree object is null or contains no data
	if (!bptree || !bptree->getRoot())
	{
		printErrorCode(500);
		return;
	}

	// Create Selection Tree
	if (!stree) {
		stree = new SelectionTree(&flog);
	}

	stree->setTree();

	// Traverse from the leftmost leaf
	BpTreeNode* node = bptree->getRoot();

	while (node && node->getMostLeftChild()) node = node->getMostLeftChild(); // Reach the leaf

	bool inserted = false;

	while (node) {
		for (auto& kv : *node->getDataMap()) {
			EmployeeData* emp = kv.second;
			if (emp->getDeptNo() == dept_no) {
				stree->Insert(emp);
				inserted = true;
			}
		}
		node = node->getNext(); // Move to the next leaf node
	}
	if (!inserted)
		printErrorCode(500); // No employees in the department
	else
		printSuccessCode("ADD_ST");
}

void Manager::ADD_ST_NAME(string name)
{
	// Check if B+ tree is empty
	if (!bptree || !bptree->getRoot()) {
		printErrorCode(500);
		return;
	}

	BpTreeNode* addNode = bptree->searchDataNode(name);
	if (!addNode)
	{ // If no node with the given name exists
		printErrorCode(500);
		return;
	}

	// Find employee with matching name
	EmployeeData* foundData = nullptr;
	for (auto& kv : *addNode->getDataMap()) {
		if (kv.second->getName() == name) {
			foundData = kv.second;
			break;
		}
	}

	if (!foundData) {
		printErrorCode(500); // Name not found
		return;
	}

	// Initialize Selection Tree
	if (!stree) {
		stree = new SelectionTree(&flog);
		stree->setTree();
	}

	// Insert into Selection Tree
	if (!stree->Insert(foundData)) {
		printErrorCode(500);
		return;
	}

	printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP()
{
	// When bptree object is null or contains no data
	if (!bptree || bptree->getRoot() == nullptr)
	{
		printErrorCode(400);
		return;
	}
	else
	{
		bptree->PrintAllData(flog);
	}
}

void Manager::PRINT_ST(int dept_no) {
	if (!stree) {
		printErrorCode(600);
		return;
	}

	if (!stree->printEmployeeData(dept_no))
		printErrorCode(600);
}

void Manager::DELETE() {
	if (!stree) {
		printErrorCode(700);
		return;
	}

	// Delete 
	if (!stree->Delete()) {
		printErrorCode(700); // Tree is empty or cannot delete
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
