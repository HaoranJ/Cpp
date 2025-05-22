#include <algorithm>       // For std::sort and std::find
#include <bitset>          // For bit manipulation
#include <cassert>         // For assertions
#include <chrono>          // For timing
#include <cmath>           // For mathematical functions
#include <cstdint>         
#include <ctime>           // For time functions
#include <deque>           
#include <fstream>         
#include <functional>      // For std::function like std::greater
#include <initializer_list> // For handling initializer lists
#include <iomanip>         
#include <iostream>        // For std::cout and std::cin
#include <limits>          // For numeric limits
#include <map>             // For std::map and multimap that allows duplicate values in sorted order
#include <mutex>           // For mutexes
#include <optional>        
#include <queue>           // For std::priority_queue
#include <random>          
#include <set>             
#include <sstream>         
#include <stdexcept>       
#include <string>          // For std::string
#include <system_error>    
#include <thread>          // For multithreading
#include <unordered_map>   
#include <unordered_set>   
#include <utility>         // For std::pair
#include <variant>         
#include <vector>          // For std::vector
#include <sys/types.h>
#include <shared_mutex> // std::shared_mutex
#include <future>

using namespace std;

struct MyComparator {
  bool operator()(const int& a, const int& b) const {
    return a > b; // descending order
  }
};

int get() { return 4; }
// Compile: g++ -std=c++17 data_structures.cpp -o ds
// Run: ./ds
int main() {
  /* std::deque*/
  std::deque<pair<vector<int>::iterator, vector<int> > > stack;
  vector<int> list0 = {1, 2, 3};
  stack.push_front({list0.begin(), list0});
  auto [it, vec] = stack.front();
  cout << "it = " << *it << ", vec = " << vec[0] << endl;
  ++it;
  // it is a copy of the iterator, not a reference.
  cout << "stack.front() still points to " << *stack.front().first << endl;
  auto& [itr_ref, list_ref] = stack.front();
  ++itr_ref;
  cout << "stack.front() now points to " << *stack.front().first << endl;

  /* unordered_map */
  std::unordered_map<std::string, int> map;
  map.insert({"A1", 1});  // Inserted
  map.insert({"A1", 2});  // No overwrite if the key already exists. It returns pair<iterator, bool>.
  map.insert_or_assign("A1", 2); // Overwrites (now map["A1"] == 2)
  map["A1"] = 2;          // Overwrites (now map["A1"] == 2)


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

  /* string */
  // append int
  int num = 44;
  std::string str = "Number: ";
  str += std::to_string(num);

  /* std::vector */
  vector<int> list;
  list.reserve(10); // list.size() == 0
  list.resize(10); // list.size() == 10

  /* Pointers */
  // int* p = &get(); // ERROR: cannot take the address of an rvalue of type 'int', get() returns a temporary value
  return 0;
}