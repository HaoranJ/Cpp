#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <stack>
#include <deque>
#include <climits>
#include <type_traits>
#include <thread>
#include <mutex>
#include "ood.h"
#include "algorithm_utils.h"
#include <set>
#include <regex>

using namespace std;

class Stats {
 public:
  int ct = 0;
};

class Vehicle {
 public:
  void LocalStaticVar() {
    std::unique_lock<std::mutex> uLock(mutex_);
    // method-local static var
    static Stats * const stats = new Stats();
    std::cout << "ct = " << stats->ct << endl;
    stats->ct++;

    // non-static var
    int i = 0;
    std::cout << "i = " << i << endl;
    ++i;
  }

 private:
  std::mutex mutex_;
};

void Setup(Vehicle* v) {
  v->LocalStaticVar();
}

int main()
{
//
//  std::vector<std::thread> threads;
//  Vehicle * v = new Vehicle();
//
//  for (int i = 0; i < 4; ++i) {
//    std::thread t(Setup, v);
//    threads.push_back(std::move(t));
//  }
//
//  for (auto & t : threads) {
//    t.join();
//  }
//
//  // Pointer bacics
//  /*
//     * DO use the indirection operator (*) to access the data stored at the address in a pointer.
//     * DO initialize all pointers either to a valid address or to null (0).
//     * Each time you allocate memory using the new keyword, you must check to make sure the pointer is not null.
//     * For every time in your program that you call new, there should be a call to delete. It is important to keep track
//     * of which pointer owns an area of memory and to ensure that the memory is returned to the free store when you are done with it.
//     * To be safe, after you delete a pointer, set it to null (0). This disarms the pointer.
//     */
//  char str1[] = "Udacity";
//  int *ptr;
//  ptr = new int;
//  *ptr = 5;
//  std::cout << *ptr << std::endl; // 5
//  std::cout << ptr << std::endl;  // address
//  delete ptr;
}