#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "ood.h"

using namespace std;

void passByRef(int& x) {
    x *= 2;
}

void passByVal(int x) {
    x *= 2;
}

int numberOfSteps (int num) {
    return num ? __builtin_popcount(num) * 2 - 1 + (32 - __builtin_clz(num) - __builtin_popcount(num)): 0;
}

/*
g++ -std=c++17 hello.cpp
*/
int main() {
    // MyDate date(1, 1);
    // date.Day(8);
    // Pointer bacics
    /*
     * DO use the indirection operator (*) to access the data stored at the address in a pointer.
     * DO initialize all pointers either to a valid address or to null (0).
     * Each time you allocate memory using the new keyword, you must check to make sure the pointer is not null.
     * For every time in your program that you call new, there should be a call to delete. It is important to keep track
     * of which pointer owns an area of memory and to ensure that the memory is returned to the free store when you are done with it.
     * To be safe, after you delete a pointer, set it to null (0). This disarms the pointer.
     */
    char str1[] = "Udacity";
    int *ptr;
    ptr = new int;
    *ptr = 5;
    std::cout << *ptr << std::endl; // 5
    std::cout << ptr << std::endl; // address
    delete ptr;
}