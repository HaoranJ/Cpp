#include <iostream>

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
  

  return 0; 
}