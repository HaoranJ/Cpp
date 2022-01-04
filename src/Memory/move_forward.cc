//
// Created by haoranjia on 12/31/21.
//
#include <string>
class Annotation {
 public:
  explicit Annotation(const std::string text) : value(std::move(text)) {}

 private:
  std::string value;
};

void StdMoveConst() {
  const std::string s = "a";
  std::string s1(std::move(s)); // it's copy, `s` still has "a"
  // const-ness has to be maintained. Moving a value out of a object
  // generally modifies the object.
  // * Don't declare object const if I want to move the object
  // * std::move casts the argument to an rvalue, but it can't guarantee
  // the result is eligible to be moved.
  std::string s2 = "b";
  std::string s3(std::move(s2)); // it's a move, `s2` has no "b"
}