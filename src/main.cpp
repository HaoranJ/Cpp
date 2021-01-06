#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iostream>
#include <ratio>
#include <chrono>
#include <thread>

#include "ood.h"

using namespace std;

int numberOfSteps(int num)
{
  return num ? __builtin_popcount(num) * 2 - 1 + (32 - __builtin_clz(num) - __builtin_popcount(num)) : 0;
}

class Node
{
  public:
  int val;
  int *p;
  Node() {}
  Node(int x) {
    val = x;
    p = new int;
    *p = x;
  }
};

void f(Node n0) {
  std::cout << "n0 address = " << &n0 << " val address = " << &(n0.val) << " p address = " << n0.p << std::endl;
}

int main()
{
  Node n0;
  Node n1(1);
  Node *n2 = new Node(2);
  std::cout << "n0 address = " << &n0 << " val address = " << &(n0.val) << " p address = " << n0.p << std::endl;
  std::cout << "n1 address = " << &n1 << " val address = " << &(n1.val) << " p address = " << n1.p << std::endl;
  std::cout << "n2 address = " << n2 << " val address = " << &(n2->val) << " p address = " << n2->p << std::endl;
  f(n0);
  f(n1);
  int *p = new int;
  *p = 1;
  int *p2 = new int;
  *p2 = 2;
  *p = *p2;

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
  std::cout << ptr << std::endl;  // address
  delete ptr;
}