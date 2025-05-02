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
#include "Memory/move_and_forward.h"
#include <set>
#include <regex>
#include <sstream>

using namespace std;

class Stats {
 public:
  ~Stats() {
    cout << "Stats d'tor" << std::endl;
  }
  int ct = 0;
};

class Vehicle {
 public:
  void LocalStaticVar() {
    std::unique_lock<std::mutex> uLock(mutex_);
    // method-local static var
    static Stats* const stats = new Stats();
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

class MyCircularQueue {
 public:
  MyCircularQueue(int k) {
    size_limit_ = k;
    for (int i = 0; i < k; ++i) {
      que_.push_back(0);
    }
  }

  bool enQueue(int value) {
    if (size_ == size_limit_) {
      return false;
    }
    rear_idx_ = (rear_idx_ + 1) % size_limit_;
    que_[rear_idx_] = value;
    ++size_;
    return true;
  }

  bool deQueue() {
    if (size_ == 0) {
      return false;
    }
    front_idx_ = (front_idx_ + 1) % size_limit_;
    --size_;
    return true;
  }

  int Front() {
    if (size_ == 0) { return -1; }
    return que_[front_idx_];
  }

  int Rear() {
    if (size_ == 0) { return -1; }
    return que_[rear_idx_];
  }

  bool isEmpty() {
    return size_ == 0;
  }

  bool isFull() {
    return size_ == size_limit_;
  }
 private:
  std::vector<int> que_;
  int size_limit_ = 0;
  int front_idx_ = 0;
  int rear_idx_ = -1;
  int size_ = 0;
};

class MyClass {
 public:
  ~MyClass() {
    cout << "MyClass d'tor." << std::endl;
  }
  string str;
  std::vector<Node> list_;
  std::vector<Node> list() {
    return list_;
  };

  std::vector<Node>& list_ref() {
    return list_;
  };

  std::vector<Node>* list_ptr() {
    return &list_;
  };
};


// Compile: g++ -std=c++11 -o main main.cpp
// Run: ./main
int main() {
  std::cout << "hello world" << std::endl;
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