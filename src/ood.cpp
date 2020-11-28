#include "ood.h"
#include <iostream>

void MyDate::Day(int d) {
  this->day = d;
}

int MyDate::Day() const {
  return this->day;
}

/*
static members are declared within their class (often in a header file) but in most cases they must be defined within the global scope.

https://stackoverflow.com/questions/9656941/why-cant-i-initialize-non-const-static-member-or-static-array-in-class/9657064
*/
int MyDate::counter{0};