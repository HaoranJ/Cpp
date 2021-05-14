#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <stack>
#include <deque>
#include <climits>
#include <type_traits>
#include "ood.h"
#include "algorithm_utils.h"

using namespace std;

bool validateStackSequences(vector<int>& pushed, vector<int>& popped) {
  stack<int> stack;
  int pop_ptr = 0, push_ptr = 0;
  while (pop_ptr < popped.size()) {
    if (!stack.empty() && popped[pop_ptr] == stack.top()) {
      stack.pop();
      ++pop_ptr;
    } else {
      while (push_ptr < pushed.size()) {
        stack.push(pushed[push_ptr]);
        ++push_ptr;
        if (pushed[push_ptr-1] == popped[pop_ptr]) { break; }
      }
      if (push_ptr == pushed.size() && pushed[push_ptr-1] != popped[pop_ptr]) {
        return false;
      }
    }
  }

  return pop_ptr == popped.size();
}

class BaseClass {
public:
   virtual void disp() {};
};
class DerivedClass: public BaseClass{
public:
   void disp() override {
      cout<<"Function of Child Class";
   }
};
class DDClass : public DerivedClass {
  public:
  void disp() override {
    cout<<"Function of Grand child Class";
  }
};

int main()
{
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