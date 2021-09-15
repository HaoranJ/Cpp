#include <stdlib.h>
#include <iostream>

/*
https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-five

The presence of user-defined constructor will prevent the implicit default constructor, 
but won't stop generation of default copy c'tor, default move c'tor, etc.

For the following five, if any is user-defined or explicitly default/delete, it will prevent the generation of the rest of 4. Thus, we need rule of five. 
The destructor: Responsible for freeing the resource once the object it belongs to goes out of scope.

The assignment operator: The default assignment operation performs a member-wise shallow copy, which does not copy the content behind the resource handle. If a deep copy is needed, it has be implemented by the programmer.

The copy constructor: As with the assignment operator, the default copy constructor performs a shallow copy of the data members. If something else is needed, the programmer has to implement it accordingly.

The move constructor: Because copying objects can be an expensive operation which involves creating, copying and destroying temporary objects, rvalue references are used to bind to an rvalue. Using this mechanism, the move constructor transfers the ownership of a resource from a (temporary) rvalue object to a permanent lvalue object.

The move assignment operator: With this operator, ownership of a resource can be transferred from one object to another. The internal behavior is very similar to the move constructor.
*/
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
    std::cout << "CREATING instance of MyMovableClass at " << this << " allocated with size = " << _size * sizeof(int) << " bytes" << std::endl;
  }

  ~MyMovableClass() // 1 : destructor
  {
    std::cout << "DELETING instance of MyMovableClass at " << this << std::endl;
    delete[] _data;
  }
  // 2 : copy constructor
  MyMovableClass(const MyMovableClass &source) 
  {
    _size = source._size;
    _data = new int[_size];
    *_data = *source._data; // deep copy
    std::cout << "COPYING content of instance " << &source << " to instance " << this << std::endl;
  }
  // 3 : copy assignment operator
  MyMovableClass &operator=(const MyMovableClass &source) 
  {
    std::cout << "ASSIGNING content of instance " << &source << " to instance " << this << std::endl;
    if (this == &source)
      return *this;
    delete[] _data;
    _data = new int[source._size]; //deep copy
    *_data = *source._data;
    _size = source._size;
    return *this;
  }

  // 4. Move constructor
  MyMovableClass(MyMovableClass &&source) { //rvalue reference
    std::cout << "MOVING (c’tor) instance " << &source << " to instance " << this << std::endl;
    _data = source._data;
    _size = source._size;
    source._data = nullptr;
    source._size = 0;
  }

  // 5. move assignment constructor
  MyMovableClass &operator=(MyMovableClass &&source) {
    std::cout << "MOVING (assign) instance " << &source << " to instance " << this << std::endl;
    if (this == &source) {
      return *this;
    }
    _data = source._data;
    _size = source._size;
    source._data = nullptr;
    source._size = 0;

    return *this;
  }
};

MyMovableClass createObject(int size) {
  /* 
  1. compiler creates temporary object in stack as an rvalue to return; 
  2. copy constructor is called on the temp object.
  */
  MyMovableClass obj(size);
  return obj; 
}

void useObject(MyMovableClass obj) { //pass by value
  std::cout << "using object " << &obj << std::endl;
}

int main()
{
  MyMovableClass obj1(10);   // regular constructor
  MyMovableClass obj2(obj1); // copy constructor
  obj2 = obj1;               // copy assignment operator

  // call to copy constructor
  // obj3 is not instantiated yet, obj1 is created, so copy assignment operator won't be called.
  MyMovableClass obj3 = obj1;

  // obj4 is not instantiated yet. Copy elision makes temp rvalue created w/o calling copy c'tor.
  MyMovableClass obj4 = createObject(10);

  // call to copy assignment operator
  // erase obj4 data; reallocate it during creation of temp object; deep copy
  obj4 = createObject(20); 

  /*** Move Semantics
  1. When heavy-weight objects need to be passed around, we use move semantics.
  2. Move semantics ensure a single object at a time has access to the resource, it's the opposite of shared-copy policy.
  */
  std::cout << "--- Move Semantics ---" << std::endl;
  MyMovableClass ms1(100); //regular c'tor
  ms1 = MyMovableClass(200); // move assignment operator. ms1 is already instantiated, '200' is a rvalue.
  MyMovableClass ms2 = MyMovableClass(300); //regular c'tor
  MyMovableClass ms3 = std::move(ms2); // move constructor. ms3 is not instantiated yet. 
  MyMovableClass ms4(std::move(MyMovableClass(400))); // also move c'tor

  std::cout << "--- Pass-by-value function ---" << std::endl;
  MyMovableClass u1(100);
  useObject(u1); //pass by value -> copy c'tor
  //pass by value -> treat lvalue as rvalue -> move c'tor
  // u1 has been invalidated in caller scope
  useObject(std::move(u1)); 
  //pass by value -> treat lvalue as rvalue -> move c'tor
  useObject(std::move(MyMovableClass(200))); 
  return 0;
}
