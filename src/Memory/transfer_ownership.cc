#include <iostream>
#include <memory>

class MyClass {
  private:
  int _member;

  public:
  MyClass(int val) : _member{val} {}
  void printVal() {
    std::cout << ", managed object " << this << " with val = " << _member << std::endl; 
  }
};

void f_unique_by_value(std::unique_ptr<MyClass> ptr) { //pass by value. get a copy from the caller
  std::cout << "unique_ptr " << &ptr;
  ptr->printVal();
}

void f_unique_by_ref(std::unique_ptr<MyClass> &ptr) {
  std::cout << "unique_ptr " << &ptr;
  ptr->printVal();
  ptr.reset();
}

// pass by value
void f_shared_by_value(std::shared_ptr<MyClass> ptr) { 
  std::cout << "shared_ptr (ref_cnt= " << ptr.use_count() << ") " << &ptr;
  ptr->printVal();
}

void f_shared_by_ref(std::shared_ptr<MyClass> &ptr) {
  std::cout << "shared_ptr (ref_cnt= " << ptr.use_count() << ") " << &ptr;
  ptr->printVal();
}

int main() {
  /*
  R. 30 : Take smart pointers as parameters only to explicitly express lifetime semantics.
  A function that does not manipulate the lifetime or ownership should use raw pointers or references instead. A function should take smart pointers as parameter only if it examines or manipulates the smart pointer itself. 
  */
  // uniquePtr is on the stack, MyClass object in on the heap.
  std::unique_ptr<MyClass> uniquePtr = std::make_unique<MyClass>(23);
  std::cout << "unique_ptr " << &uniquePtr; 
  uniquePtr->printVal();

  f_unique_by_value(std::move(uniquePtr)); //move c'tor

  if (uniquePtr) { //uniquePtr is now an empty shell w/o object to manage
    uniquePtr->printVal();
  }

  /* output
  unique_ptr 0x7ffe25403388, managed object 0x5644f3272e70 with val = 23
  unique_ptr 0x7ffe25403398, managed object 0x5644f3272e70 with val = 23
  */

  /*
  R.34: Take a shared_ptr parameter to express that a function is part owner
  Move semantics are usually not needed when using shared pointers.
  */
  std::shared_ptr<MyClass> sharedPtr = std::make_shared<MyClass>(23);
  std::cout << "shared_ptr (ref_cnt= " << sharedPtr.use_count() << ") " << &sharedPtr;
  sharedPtr->printVal();

  f_shared_by_value(sharedPtr);

  std::cout << "shared_ptr (ref_cnt= " << sharedPtr.use_count() << ") " << &sharedPtr;
  sharedPtr->printVal();

  /*
  R.33: Take a unique_ptr& parameter to express that a function reseats the widget
  */
  std::cout << "unique by ref...." << std::endl;
  std::unique_ptr<MyClass> uniquePtr1 = std::make_unique<MyClass>(24);
  std::cout << "unique_ptr " << &uniquePtr1; 
  uniquePtr1->printVal();

  f_unique_by_ref(uniquePtr1); 

  if (uniquePtr1) { //uniquePtr1 is now an empty shell w/o object to manage
    uniquePtr1->printVal();
  }

  /*
  R.35: Take a shared_ptr& parameter to express that a function might reseat the shared pointer
  */
  std::cout << "shared_ptr by ref...." << std::endl;
  std::shared_ptr<MyClass> sharedPtr1 = std::make_shared<MyClass>(24);
  std::cout << "shared_ptr (ref_cnt= " << sharedPtr1.use_count() << ") " << &sharedPtr1;
  sharedPtr1->printVal();

  f_shared_by_ref(sharedPtr1);

  std::cout << "shared_ptr (ref_cnt= " << sharedPtr1.use_count() << ") " << &sharedPtr1;
  sharedPtr1->printVal();
  return 0;
}