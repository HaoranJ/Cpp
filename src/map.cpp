//
// Created by Haoran Jia on 5/2/21.
//
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <deque>
#include <iostream>

int map() {
  std::unordered_map<int, int> map;
  // inserts
  map.insert(std::make_pair(1, 0));

  // contains
  std::unordered_map<int, int>::iterator itr = map.find(1);
  if (itr == map.end()) {
    std::cout << "Not Found";
  } else {
    std::cout << itr->second;
  }

  return 0;
}
