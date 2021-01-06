#include <iostream>

int main() {
  /*
  Programers need to take special care when capturing local objects by reference because a Lambda’s lifetime may exceed the lifetime of its capture list: It must be ensured that the object to which the reference points is still in scope when the Lambda is called. This is especially important in multi-threading programs.
  */
  int id = 0;
  // id is captured by value
  auto f1 = [id]() { std::cout << "ID = " << id << std::endl; };
  // id is captured by ref
  auto f2 = [&id]() { std::cout << "ID = " << id << std::endl; };

  // Error. id may not be modified.
  // auto f3 = [id]() { std::cout << "ID = " << ++id << std::endl; };

  auto f4 = [&id]() mutable { std::cout << "ID = " << ++id << std::endl; };

  // execute lambdas
  f1(); // closure, exists in runtime
  f2();
  f4();

  int x = 1;
  auto f5 = [](const int x) { std::cout << "ID = " << x << std::endl; };
  f5(x);

  return 0;
}