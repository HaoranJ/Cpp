//
// Created by haoranjia on 3/31/22.
//

class MemberVar {

};

class Wrapper {
  // When Wrapper object is constructed,
  // When Wrapper object is destructed,
 private:
  MemberVar third_destroyed_; // first created, last destructed
 public:
  MemberVar second_destroyed_;
 private:
  MemberVar first_destroyed_; // last created, first destructed
};
