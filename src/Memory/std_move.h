//
// Created by haoranjia on 4/23/22.
//

#ifndef SRC_MEMORY_STD_MOVE_H_
#define SRC_MEMORY_STD_MOVE_H_
#include <string>
class Foo {

};

void move() {
  // std::move() doesn't actually move anything. It only casts "to be moved"
  // object to rvalue ref and let the compiler do what it sees fit.
  Foo f0;
  Foo f1 = std::move(f0); // If Foo doesn't have a move c'tor, it will be copied.
  // "Moving" resource becomes especially important for move-only-cannot-copy objects
  // like std::unique_ptr<>
}
#endif //SRC_MEMORY_STD_MOVE_H_
