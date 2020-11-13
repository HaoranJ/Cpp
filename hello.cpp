#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

void passByRef(int& x) {
    x *= 2;
}

void passByVal(int x) {
    x *= 2;
}

class SimpleCat {
public:
    SimpleCat();
    ~SimpleCat();
    void SetAge(int a) { this->age = a; } // compiler takes care of 'this' pointer's creation and deletion.
    int GetAge() { return age; }
private:
    int age;
};

SimpleCat::SimpleCat() {
    cout << "Constructor called\n";
    age = 1;
}

SimpleCat::~SimpleCat() {
    cout << "Destructor called\n";
}

int numberOfSteps (int num) {
    return num ? __builtin_popcount(num) * 2 - 1 + (32 - __builtin_clz(num) - __builtin_popcount(num)): 0;
}

void strayPointer() {
    int * pInt = new int;
    *pInt = 7;
    delete pInt;
    // point stray pointer to NULL
    // pInt = NULL;
    long* pLong = new long;
    *pLong = 90000;
    long* pLong2 = new long;
    *pLong2 = 100000;
    cout << "*pLong = " << *pLong << endl;
    cout << "*pLong2 = " << *pLong2 << endl;
    *pInt = 20; // 20 is assigned to stray(dangling) pointer.
    cout << "*pInt = " << *pInt << endl;
    cout << "*pLong = " << *pLong << endl;
    cout << "*pLong2 = " << *pLong2 << endl;
    delete pLong;
}

int main() {
    strayPointer();
    SimpleCat * cat = new SimpleCat;
    // access data members
    cout << cat->GetAge() << endl;
    cout << (*cat).GetAge() << endl;
    delete cat;
    // Pointer bacics
    /*
     * DO use the indirection operator (*) to access the data stored at the address in a pointer.
     * DO initialize all pointers either to a valid address or to null (0).
     * Each time you allocate memory using the new keyword, you must check to make sure the pointer is not null.
     * For every time in your program that you call new, there should be a call to delete. It is important to keep track
     * of which pointer owns an area of memory and to ensure that the memory is returned to the free store when you are done with it.
     * To be safe, after you delete a pointer, set it to null (0). This disarms the pointer.
     */
    int *ptr;
    ptr = new int;
    *ptr = 5;
    cout << *ptr << endl; // 5
    cout << ptr << endl; // address
    delete ptr;

    // Pass by reference
    int x = 2;
    passByRef(x);
    int y = 2;
    passByVal(y);

    // address of
    long xx = -65;
    cout << "xx:\t" << xx;
    cout << "address of xx:\t" << &xx << endl;

    int *pAge = new int;
    *pAge = 20;
    delete pAge; // avoid memory leak
    pAge = new int;
    *pAge = 7;
    cout << "pAge = " << *pAge << endl;
    delete pAge;
}