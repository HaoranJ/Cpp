#include <iostream>
#include <memory>

void RawPointer() {
  int *raw = new int; //allocate memory on the heap, store address in the pointer variable
  *raw = 1;
  delete raw;
}

void UniquePointer() {
  std::unique_ptr<int> uni_ptr = std::make_unique<int>(); // create a unique pointer on the stack
  *uni_ptr = 1;
  // delete not needed
}

void SharedPointer() {
  std::shared_ptr<int> shared1 = std::make_shared<int>();
  std::cout << "shared pointer count = " << shared1.use_count() << std::endl;
  {
    std::shared_ptr<int> shared2 = shared1;
    std::cout << "shared pointer count = " << shared1.use_count() << std::endl;
  }

  std::cout << "shared pointer count = " << shared1.use_count() << std::endl;
}

class MyClass {
  public:
  ~MyClass() { std::cout << "Destructor of MyClass called" << std::endl; }
};

/* Weak pointers hold a non-owning reference to an object 
that's managed by another shared pointer. 
Weak pointers don't increase reference count.
Weak pointers can only be created out of weak or shared pointers.
*/
void WeakPointer() {
  std::shared_ptr<int> sp = std::make_shared<int>();
  std::cout << "shared pointer count = " << sp.use_count() << std::endl; // 1
  std::weak_ptr<int> weak1(sp);
  std::weak_ptr<int> weak2(weak1);
  std::cout << "shared pointer count = " << sp.use_count() << std::endl; // 1

  sp.reset(new int);
  if (weak1.expired()) {
    std::cout << "Weak pointer expired!" << std::endl;
  }

  // std::weak_ptr<int> weak3(new int); //error
}

void ConvertBetweenPointers() {
  std::unique_ptr<int> uniquePtr = std::make_unique<int>();
  // (1) shared pointer from unique pointer
  // move assignment operator - steal resource from uniquePtr and invalidate uniquePtr's resource handlers.
  std::shared_ptr<int> sharedPtr1 = std::move(uniquePtr);

  // (2) shared pointer from weak pointer
  std::weak_ptr<int> weakPtr(sharedPtr1);
  std::shared_ptr<int> sharedPtr2 = weakPtr.lock();

  // (3) no options for converting away from shared pointers.

  // (4) raw pointers
  int * rawPtr = sharedPtr2.get(); 
}

int main() {
  SharedPointer();

  std::shared_ptr<MyClass> shared = std::make_shared<MyClass>();
  std::cout << "shared pointer count = " << shared.use_count() << std::endl;

  shared.reset(new MyClass);
  std::cout << "shared pointer count = " << shared.use_count() << std::endl;
}