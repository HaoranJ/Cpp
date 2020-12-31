#include <stdlib.h>
#include <iostream>

class MyMovableClass
{
private:
  int _size;
  int *_data;

public:
  MyMovableClass(size_t size) // constructor
  {
      _size = size;
      _data = new int[_size];
      std::cout << "CREATING instance of MyMovableClass at " << this << " allocated with size = " << _size*sizeof(int)  << " bytes" << std::endl;
  }

  ~MyMovableClass() // 1 : destructor
  {
      std::cout << "DELETING instance of MyMovableClass at " << this << std::endl;
      delete[] _data;
  }

  // copy constructor
  MyMovableClass(const MyMovableClass &source) {
    _size = source._size;
    _data = new int[_size];
    *_data = *source._data; // deep copy
    std::cout << "COPYING content of instance " << &source << " to instance " << this << std::endl;
  }

  MyMovableClass & operator=(const MyMovableClass & source) {
    std::cout << "ASSIGNING content of instance " << &source << " to instance " << this << std::endl;
    if (this == &source) {
      return *this;
    }
    _size = source._size;
    _data = new int[_size];
    *_data = *source._data; // deep copy
    return *this;
  }
};

int main() {
  MyMovableClass obj1(10);
  MyMovableClass obj2(obj1);
  obj2 = obj1;
  return 0;
}