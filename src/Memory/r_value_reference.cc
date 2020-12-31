#include <iostream>

/*
The object that binds to the rvalue reference &&val is yours, it is not needed anymore within the scope of the caller (which is main)

1. Passing values like this improves performance as no temporary copy needs to be made anymore and
2. ownership changes, since the object the reference binds to has been abandoned by the caller and now binds to a handle which is available only to the receiver. This could not have been achieved with lvalue references as any change to the object that binds to the lvalue reference would also be visible on the caller side.
*/
void myFunc(int &&val) { // rvalue reference
  // within the scope of myFunction, val is an lvalue as it gives access to the memory location where the number 42 is stored.
  std::cout << val << std::endl;
}

int main()
{
  int x = 1;
  int &y = x; // y points to the existing x;
  x++; // 2
  y++; // 3

  int i = 1; 
  int j = 2; 
  /* 
  addition of (i+j) is created as rvalue;
  copy it to lvalue k;
  delete the rvalue;
  */
  int k = i + j;  
  // rvalue reference - hold the temporary memory of addition here.
  int &&l = i + j;
  std::cout << "k = " << k << ", l = " << l << std::endl;

  myFunc(44);

  int x = 23;
  myFunc(std::move(x)); //convert lvalue into rvalue.

  return 0; 
}