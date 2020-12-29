#include <stdlib.h>
#include <iostream>

class MyClass
{
private:
    int *_number;

public:
    MyClass()
    {
        std::cout << "Allocate memory\n";
        _number = (int *)malloc(sizeof(int));
    }
    ~MyClass()
    {
        std::cout << "Delete memory\n";
        free(_number);
    }
    void setNumber(int number)
    {
        *_number = number;
        std::cout << "Number: " << _number << "\n";
    }
};

int main() {
  /* placement new
  If we were to use the standard new/delete construct, memory would be allocated and freed unnecessarily as only the content of the memory block changes but not its size. By separating allocation from construction, we can get a significant performance increase.
  malloc a heap block at first, then construct an object 
  at the memory location. 
  */
  void *memory = malloc(sizeof(MyClass));
  MyClass *object = new (memory) MyClass; //placement new
  object->~MyClass();
  free(memory);
}