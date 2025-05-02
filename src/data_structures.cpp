#include <cassert>      // For assertions
#include <vector>      // For std::vector
#include <unordered_map>
#include <unordered_set>
#include <map> // For std::map and multimap that allows duplicate values in sorted order
#include <set>
#include <queue>       // For std::priority_queue
#include <deque>
#include <iostream>     // For std::cout and std::cin
#include <algorithm>   // For std::sort and std::find
#include <string>      // For std::string
#include <utility>     // For std::pair
#include <functional>  // For std::function like std::greater
#include <cmath>       // For mathematical functions
#include <ctime>       // For time functions
#include <limits>      // For numeric limits
#include <thread>      // For multithreading
#include <mutex>       // For mutexes
#include <chrono>      // For timing
#include <cassert>     // For assertions
#include <bitset>      // For bit manipulation
#include <initializer_list> // For handling initializer lists

using namespace std;

struct MyComparator {
  bool operator()(const int& a, const int& b) const {
    return a > b; // descending order
  }
};

int main() {
  /* map */
  // descending order
  std::map<int, string, std::greater<int>> m0;
  std::map<int, string, MyComparator> m;
  m[1] = "one";
  m[2] = "two";
  m[3] = "three";

  for (const auto& pair : m) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }

  return 0;
}