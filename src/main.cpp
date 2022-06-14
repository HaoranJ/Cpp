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

vector<int> dirs = {0, 1, 2, 3}; // {up, right, down, left}
vector<pair<int, int>> best_solution;
int row_num = 8;
int col_num = 8;
bool ValidMove(int& r, int& c, vector<vector<int>>& layout, int dir) {
  switch (dir) {
    case 0:
      for (int i = r - 1; i >= 0; --i) {
        if (layout[i][c] > 0) {
          r = i + 1;
          return true;
        }
      }
      return false;
    case 1:
      for (int i = c + 1; i < col_num; ++i) {
        if (layout[r][i] > 0) {
          c = i - 1;
          return true;
        }
      }
      return false;
    case 2:
      for (int i = r + 1; i < row_num; ++i) {
        if (layout[i][c] > 0) {
          r = i - 1;
          return true;
        }
      }
      return false;
    case 3:
      for (int i = c - 1; i >= 0; --i) {
        if (layout[r][i] > 0) {
          c = i + 1;
          return true;
        }
      }
      return false;
    default:return false;
  }
}

string LayoutToString(vector<vector<int>>& layout) {
  std::stringstream ss;
  for (int r = 0; r < row_num; ++r) {
    for (int c = 0; c < col_num; ++c) {
      if (layout[r][c] == 0) { continue; }
      ss << "(" << r << "," << c << "):" << layout[r][c] << ";";
    }
  }
  return ss.str();
}

std::unordered_set<string> visited;

class Snapshot {
 public:
  Snapshot(vector<vector<int>>& layout, vector<pair<int, int>>& moves)
      : layout_(layout), moves_(moves) {

  }

  vector<vector<int>> layout_;
  vector<pair<int, int>> moves_;
};

vector<pair<int, int>> BestMoves_BFS(vector<vector<int>>& layout,
                                     pair<int, int>& dest) {
  vector<pair<int, int>> moves;
  Snapshot start(layout, moves);
  deque<Snapshot> que;
  que.push_back(start);
  int steps = 0;
  while (que.size() > 0) {
    int level_size = que.size();
    for (int i = 0; i < level_size; ++i) {
      auto snapshot = que.front();
      que.pop_front();
      visited.insert(LayoutToString(snapshot.layout_));
      for (int r = 0; r < row_num; ++r) {
        for (int c = 0; c < col_num; ++c) {
          if (snapshot.layout_[r][c] == 0) {
            continue;
          }
          int initial_num = snapshot.layout_[r][c];
          for (int dir: dirs) {
            int cur_r = r;
            int cur_c = c;
            if (ValidMove(cur_r, cur_c, snapshot.layout_, dir)) {
              // no move.
              if (r == cur_r && c == cur_c) {
                continue;
              }
              snapshot.moves_.push_back({initial_num, dir});
              // finished
              if (snapshot.layout_[r][c] == 1 && cur_r == dest.first
                  && cur_c == dest.second) {
                return snapshot.moves_;
              }
              Snapshot next_snapshot(snapshot.layout_, snapshot.moves_);
              next_snapshot.layout_[cur_r][cur_c] = initial_num;
              next_snapshot.layout_[r][c] = 0;
              auto layout_str = LayoutToString(next_snapshot.layout_);
              if (visited.find(layout_str) == visited.end()) {
                que.push_back(next_snapshot);
              }
              snapshot.moves_.pop_back();
            }
          }
        }
      }
    }
    ++steps;
  }

  return moves;
}

void Recur(vector<vector<int>>& layout,
           pair<int, int>& dest,
           vector<pair<int, int>>& moves) {
  std::cout << "Recur visited = " << visited.size() << std::endl;
  visited.insert(LayoutToString(layout));
  // base cases
  // recursive step
  for (int r = 0; r < row_num; ++r) {
    for (int c = 0; c < col_num; ++c) {
      if (layout[r][c] == 0) {
        continue;
      }
      for (int dir: dirs) {
        int cur_r = r;
        int cur_c = c;
        if (ValidMove(cur_r, cur_c, layout, dir)) {
          if (r == cur_r && c == cur_c) {
            continue;
          }
          if (layout[r][c] == 1 && cur_r == dest.first
              && cur_c == dest.second) {
            if (best_solution.empty() || best_solution.size() > moves.size()) {
              best_solution = moves;
            }
            continue;
          }
          layout[cur_r][cur_c] = layout[r][c];
          layout[r][c] = 0;
          moves.push_back({layout[cur_r][cur_c], dir});
          if (visited.find(LayoutToString(layout)) == visited.end()) {
            Recur(layout, dest, moves);
          }
          moves.pop_back();
          layout[r][c] = layout[cur_r][cur_c];
          layout[cur_r][cur_c] = 0;
        }
      }
    }
  }
}

vector<pair<int, int>> BestMoves(vector<vector<int>>& layout,
                                 pair<int, int>& dest) {
  vector<pair<int, int>> moves;
  Recur(layout, dest, moves);
  return best_solution;
}

string PrintAnswerString(vector<pair<int, int>>& solution) {
  for (const auto& pair: solution) {
    string dir = "";
    if (pair.second == 0) { dir = "up"; }
    else if (pair.second == 1) { dir = "right"; }
    else if (pair.second == 2) { dir = "down"; }
    else if (pair.second == 3) { dir = "left"; }
    std::cout << pair.first << ":" << dir << std::endl;
  }
  return "";
}

class Wrapper {
 public:
  Wrapper() {
    my_class_.str = "test";
    node_.val = 0;
    stats_.ct = 1;
  }
 private:
  MyClass my_class_;
 public:
  Node node_;
 private:
  Stats stats_;
};

void print_num(int i) {
  std::cout << i << std::endl;
}

string removeDuplicates(string s) {
  while (s.size() > 0) {
    int initial_size = s.size();
    int i = 0;
    while (i < s.size()-1) {
      // 012345 -> 0123 -> 01
      // abbycc -> aycc -> ay
      if (s[i] == s[i+1]) {
        s.erase(i, 2);
      } else {
        ++i;
      }
    }
    if (initial_size == s.size()) {
      break;
    }
  }
  return s;
}

int main() {
  std::vector<int> arr = {1,2,2,1,1,3};
  std::function<void(int)> f0 = print_num;
  f0(7);
  std::function<void()> f1 = []() { print_num(8); };
  f1();
  std::function<void()> f_bind = std::bind(print_num, 1111);
  f_bind();
  auto factorial = [](int n) {
    std::function<int(int)> fac = [&fac](int n) { return n < 2 ? 1 : n * fac(n-1); };
    return fac(n);
  };
  std::cout << "7! = " << factorial(7) << std::endl;
//  vector<vector<int>> layout0 = {{0, 0, 0, 0, 0, 0, 0, 2},
//                                 {0, 1, 0, 0, 0, 3, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {0, 4, 0, 0, 0, 0, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 5},
//                                 {0, 0, 0, 0, 0, 6, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {7, 0, 8, 0, 0, 9, 0, 0}};
//  pair<int, int> dest = {4, 4};
//  auto solution = BestMoves_BFS(layout0, dest);
//  PrintAnswerString(solution);

//  vector<vector<int>> layout1 = {{0, 0, 0, 0, 0, 0, 0, 2},
//                                 {0, 1, 0, 0, 0, 3, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {0, 4, 0, 0, 0, 0, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 5},
//                                 {0, 0, 0, 0, 0, 6, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {7, 0, 8, 0, 0, 9, 0, 0}};
//  pair<int, int> dest1 = {4, 4};
//  auto solution1 = BestMoves_BFS(layout1, dest1);
//  PrintAnswerString(solution1);

//  vector<vector<int>> layout2 = {{0, 2, 0, 0, 0, 0, 3, 0},
//                                 {0, 0, 0, 0, 0, 4, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {0, 0, 0, 1, 0, 0, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {0, 0, 0, 0, 5, 0, 0, 0},
//                                 {0, 0, 0, 0, 0, 0, 0, 0},
//                                 {6, 0, 7, 0, 0, 0, 0, 8}};
//  pair<int, int> dest2 = {4, 4};
//  auto solution2 = BestMoves_BFS(layout2, dest2);
//  PrintAnswerString(solution2);

//  MyCircularQueue q(3);
//  q.enQueue(1);
//  q.enQueue(2);
//  q.enQueue(3);
//  q.enQueue(4);
//  int r = q.Rear();
//  bool full = q.isFull();
//  q.deQueue();
//  q.enQueue(5);
//  int r1 = q.Rear();
//  StdForward();

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