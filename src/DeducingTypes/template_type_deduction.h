//
// Created by haoranjia on 6/14/22.
//

#ifndef SRC_DEDUCINGTYPES_TEMPLATE_TYPE_DEDUCTION_H_
#define SRC_DEDUCINGTYPES_TEMPLATE_TYPE_DEDUCTION_H_

template<typename T>
void f3(T param); // param is passed by value.

void case_3() {
  int z = 27;
  const int cz = z;
  const int& rz = z;
  f3(z); // T's type is int, param's type is int
  f3(cz); // T - int, param - int
  f3(rz); // T - int, param - int

  const char* const ptr = "Fun with pointers";
  f3(ptr); // T and param - const char*. We dropped const-ness of the pointer,
  // but we keep the const-ness of the object that ptr points to.
}


#endif //SRC_DEDUCINGTYPES_TEMPLATE_TYPE_DEDUCTION_H_
