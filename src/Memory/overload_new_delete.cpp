#include <stdlib.h>
#include <iostream>

/*
Reasons for overloading new and delete
The overloaded new operator function allows to add additional parameters. Therefore, a class can have multiple overloaded new operator functions. This gives the programmer more flexibility in customizing the memory allocation for objects.

Overloaded the new and delete operators provides an easy way to integrate a mechanism similar to garbage collection capabilities (such as in Java), as we will shorty see later in this course.

By adding exception handling capabilities into new and delete, the code can be made more robust.

It is very easy to add customized behavior, such as overwriting deallocated memory with zeros in order to increase the security of critical application data.
*/
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

    // overload new
    void * operator new (size_t size) {
      std::cout << "new: Allocating " << size << " bytes of memory" << std::endl;
      void *p = malloc(size);
      return p;
    }

    // overload new - accept array
    void * operator new[] (size_t size) {
      std::cout << "new: Allocating " << size << " bytes of memory" << std::endl;
      void *p = malloc(size);
      return p;
    }

    // overload delete
    void operator delete(void *p) {
      std::cout << "delete: Memory is freed again " << std::endl;
      free(p);
    }

    // overload delete - accept array
    void operator delete[](void *p) {
      std::cout << "delete: Memory is freed again " << std::endl;
      free(p);
    }
};

int main() {
  MyClass *p = new MyClass();
  delete p;

  MyClass *pa = new MyClass[4]();
  delete[] pa; //instead of delete pa
}