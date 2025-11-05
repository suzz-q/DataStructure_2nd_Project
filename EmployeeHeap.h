#pragma once

#include "EmployeeData.h"

class EmployeeHeap {
private:
    
    EmployeeData** heapArr;
    int maxCapacity = 10;
    int size;

public:
    EmployeeHeap() {
        heapArr = new EmployeeData*[maxCapacity+1];
        
        size = 0;
        for (int i = 0; i <= maxCapacity; i++)
        {
            heapArr[i] = nullptr;
        }
    }
    ~EmployeeHeap() {
        delete[] heapArr;
    }
    
    void Insert(EmployeeData* data);
    EmployeeData* Top(); //topÀÌ max°ª
    bool Delete();

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();
    int getSize() { return size; }
    EmployeeData** getHeapArr() { return heapArr; }

};