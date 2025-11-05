#pragma once
#include "EmployeeHeap.h"

//Max Heap, income 기준 정렬, 포인터 배열 구현, 동적 할당 

void EmployeeHeap::Insert(EmployeeData* data) 
{
    //heap이 꽉 찬 경우-> doubling
    if (size+1 > maxCapacity)
    {
        ResizeArray();
    }

    heapArr[++size] = data;
    UpHeap(size);
    
}

EmployeeData* EmployeeHeap::Top() 
{
    if (IsEmpty())
    {
        return nullptr;
    }
    return heapArr[1];
}

bool EmployeeHeap::Delete() 
{
    if (IsEmpty())
    {
        return false;
    }

    //맨 밑에 있는 것을 root로 가져와서 비교하고 down을 반복함
    EmployeeData* removed = heapArr[1];
    heapArr[1] = heapArr[size];
    heapArr[size] = nullptr;
    size--;

    DownHeap(1);

    return true;

}
 bool EmployeeHeap::IsEmpty() 
 {
     if (size==0)
     {
         return true;
     }
     return false;
}

void EmployeeHeap::UpHeap(int index) 
{
    while (index > 1)
    {
        int parent = index / 2;
        if (heapArr[parent]->getIncome() >= heapArr[index]->getIncome())
        {
            break;
        }
        //temp써서 swap 안 쓰는 걸로 바꿔도 되지 않나
        EmployeeData* temp = heapArr[parent];
        heapArr[parent] = heapArr[index];
        heapArr[index] = temp;

        index = parent;
   }
}

void EmployeeHeap::DownHeap(int index) 
{
    while (index * 2 <= size)
    {
        int left = index * 2;
        int right = index * 2 + 1;
        int larger = left;

        if (right <= size && heapArr[right]->getIncome() > heapArr[left]->getIncome())
        {
            larger = right;
        }

        if (heapArr[index]->getIncome() >= heapArr[larger]->getIncome())
        {
            break;
        }

        swap(heapArr[index], heapArr[larger]);
        index = larger;
    }
}

//doubling 함수
void EmployeeHeap::ResizeArray() 
{
    EmployeeData** temp = new EmployeeData * [2 * maxCapacity + 1];
    for (int i = 0; i <= 2 * maxCapacity; i++)
        temp[i] = nullptr;

    
    //copy(heapArr+1, heapArr + size + 1, temp+1);
    for (int i = 1; i <= size; i++)
        temp[i] = heapArr[i];

    delete[] heapArr;
    heapArr = temp;
    maxCapacity *= 2;
}