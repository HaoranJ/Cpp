//
// Created by haoranjia on 1/28/22.
//
#include <vector>
#include <string>
#include <map>

void Initializer_List() {
  // vector has a constructor vector(std::initializer_list<T> values)
  // 1. Detect the type of the values in the initializer list, which is int.
  // 2. Copy the values from the list into an array of five ints.
  // 3. Construct an initializer_list<int> object that "wraps" the array created in the preceding step.
  // 4. Pass the initializer_list<int> object by reference to list's sequence constructor.
  // The constructor in turn allocates n elements in the vector object, initializing them with the values of the array.
  std::vector<int> list = {1,2,3,4,5};
  // map
  // old school
  //  std::map<int, std::string> m = {
  //    std::pair<const int, std::string>(1, "a"),
  //    std::pair<const int, std::string>(2, "b"),
  //    std::pair<const int, std::string>(3, "c")
  //  };
  // uniform initialization syntax
  std::map<int, std::string> map = {{1, "a"}, {2, "b"}};

  for (int test_val : {1, 10, 50}) {
    // Do some test
  }
}

