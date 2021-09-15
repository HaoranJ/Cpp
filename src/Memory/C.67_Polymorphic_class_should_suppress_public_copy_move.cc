#include <iostream>
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-copy-virtual
class B {
  public:
  B() = default;
  B(const B&) = delete; // copy c'tor
  B& operator=(const B&) = delete; // copy assignment operator
  B(B&&) = delete; // move c'tor
  B& operator=(B&&) = delete; // move assignment operator
  virtual char m() { return 'B'; }
};

class D : public B {
  public:
  char m() override { return 'D'; }
};

void f(B& b) {
  auto b2 = b;
  std::cout << b2.m() << std::endl; // error: use of deleted function ‘B::B(const B&)’
}

int main() {
  D d;
  f(d);
}