#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
class Manager {
private:
	char* cmd;
	BpTree* bptree;
	SelectionTree* stree;

public:
	Manager(int bpOrder) {		//constructor
		bptree = new BpTree(bpOrder);
		stree = nullptr;
	}


	~Manager() {				//destructor
		
	}

	ifstream fin;
	ofstream flog;

	void run(const char* command);
	void LOAD();
	void ADD_BP(string name, int dept_no, int ID, int income);
	void SEARCH_BP_NAME(string name);
	bool SEARCH_BP_RANGE(string start, string end);
	void PRINT_BP();

	void ADD_ST_DEPTNO(int dept_no);
	void ADD_ST_NAME(string name);
	void PRINT_ST(int dept_no);
	void DELETE();

	void printErrorCode(int n);
	void printSuccessCode(string success);
};

