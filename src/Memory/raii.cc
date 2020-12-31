#include <iostream>

class MyInt {
  int *_p;

  public:
  MyInt(int *p = NULL) { _p = p; }
  
  ~MyInt() {
    std::cout << "resource " << *_p << " deallocated" << std::endl;
    delete _p;
  }

  // overload dereference operator
  int &operator*() { return *_p; }
};

int main() {
  double den[] = {1.0, 2.0, 3.0, 4.0, 5.0};
  for (size_t i = 0; i < 5; i++) {
    /* 1. new int(i) creates a new block of memory on the heap
    and returns the address of memory
    2. MyInt en(..) creates an object of class on the stack which internally
    has _p pointing to the address of int memory block.
    Since MyInt object is on the stack, it'd be automatically deallocated after going out of the scope. 
    */
    MyInt en(new int(i));

    // use the resource
    std::cout << *en << "/" << den[i] << " = " << *en / den[i] << std::endl;
  }

  for (size_t i = 0; i < 5; i++) {
    // allocate the resource on the heap instead of stack
    MyInt *en = new MyInt(new int(i));

    // use the resource
    std::cout << **en << "/" << den[i] << " = " << **en / den[i] << std::endl;
  }

  return 0;
}