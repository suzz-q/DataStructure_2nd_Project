#pragma once
#include "EmployeeHeap.h"

// Max Heap: Sorted in descending order based on income
// Implemented using a pointer array with dynamic allocation

void EmployeeHeap::Insert(EmployeeData* data)
{
    // If the heap is full, double the array size (dynamic reallocation)
    if (size + 1 > maxCapacity)
    {
        ResizeArray();
    }

    // Insert new data at the end and reorder using UpHeap
    heapArr[++size] = data;
    UpHeap(size);
}

EmployeeData* EmployeeHeap::Top()
{
    // Return nullptr if the heap is empty
    if (IsEmpty())
    {
        return nullptr;
    }
    // Return the root (employee with the highest income)
    return heapArr[1];
}

bool EmployeeHeap::Delete()
{
    // Cannot delete if the heap is empty
    if (IsEmpty())
    {
        return false;
    }

    // Delete the root, move the last element to the root, and reorder using DownHeap
    EmployeeData* removed = heapArr[1];
    heapArr[1] = heapArr[size];
    heapArr[size] = nullptr;
    size--;

    DownHeap(1);
    return true;
}

bool EmployeeHeap::IsEmpty()
{
    // Return true if heap size is 0
    return (size == 0);
}

void EmployeeHeap::UpHeap(int index)
{
    // Move the newly inserted node up while its income is greater than its parent's
    while (index > 1)
    {
        int parent = index / 2;
        // Stop if the parent has greater or equal income (heap order maintained)
        if (heapArr[parent]->getIncome() >= heapArr[index]->getIncome())
        {
            break;
        }

        // Swap parent and child nodes
        EmployeeData* temp = heapArr[parent];
        heapArr[parent] = heapArr[index];
        heapArr[index] = temp;

        index = parent; // Move up to parent position and continue comparison
    }
}

void EmployeeHeap::DownHeap(int index)
{
    // Start from the root and move down comparing with child nodes
    while (index * 2 <= size)
    {
        int left = index * 2;
        int right = index * 2 + 1;
        int larger = left;

        // If the right child exists and has greater income, use the right child
        if (right <= size && heapArr[right]->getIncome() > heapArr[left]->getIncome())
        {
            larger = right;
        }

        // Stop if parent has greater or equal income than child
        if (heapArr[index]->getIncome() >= heapArr[larger]->getIncome())
        {
            break;
        }

        // Swap parent and child
        swap(heapArr[index], heapArr[larger]);
        index = larger; // Continue checking from the new position
    }
}

// Function to double the array size (Doubling)
void EmployeeHeap::ResizeArray()
{
    // Create a new array of double the previous size + 1
    EmployeeData** temp = new EmployeeData * [2 * maxCapacity + 1];
    for (int i = 0; i <= 2 * maxCapacity; i++)
        temp[i] = nullptr;

    // Copy existing data
    for (int i = 1; i <= size; i++)
        temp[i] = heapArr[i];

    // Delete the old array and replace it
    delete[] heapArr;
    heapArr = temp;
    maxCapacity *= 2; // Update maximum capacity
}
