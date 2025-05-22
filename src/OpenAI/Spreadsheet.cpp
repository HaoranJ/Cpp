#include <shared_mutex>
#include <algorithm> // For std::sort and std::find
#include <bitset>    // For bit manipulation
#include <cassert>   // For assertions
#include <chrono>    // For timing
#include <cmath>     // For mathematical functions
#include <cstdint>
#include <ctime> // For time functions
#include <deque>
#include <exception>
#include <fstream>
#include <functional>       // For std::function like std::greater
#include <initializer_list> // For handling initializer lists
#include <iomanip>
#include <iostream> // For std::cout and std::cin
#include <limits>   // For numeric limits
#include <map>      // For std::map and multimap
#include <mutex>    // For mutexes
#include <optional>
#include <queue> // For std::priority_queue
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string> // For std::string
#include <sys/types.h>
#include <system_error>
#include <thread> // For multithreading
#include <unordered_map>
#include <unordered_set>
#include <utility> // For std::pair
#include <variant>
#include <vector> // For std::vector
using namespace std;

/**
 * Interview Question:
 *
 * Design a spreadsheet where each cell can contain a value or a formula
 * referencing other cells. For simplicity, assume the formula only has “=” and
 * “+” operators, for example, spreadsheet.SetCell('A3', '=A1+A2').
 *
 * Part 1: Suboptimal GetCell Implementation
 *    - In this phase, the value is computed on the fly whenever GetCell is
 * called.
 *    - This is simple but inefficient for deep dependency chains as every call
 * recomputes values.
 *    - No caching is done; it walks the dependency graph every time.
 *
 * Part 2: Optimized SetCell with Efficient Updates
 *    - In this phase, when SetCell is called, all impacted cells are updated
 * immediately.
 *    - GetCell becomes O(1) as it only returns the stored value.
 *    - We propagate the updates efficiently using a BFS mechanism to all
 * dependent cells.
 *
 * Requirements:
 *    1. Handle Dependencies:
 *        - If cell A1 depends on A2 and A3, changing A2 or A3 should
 * automatically update A1.
 *
 *    2. Prevent Cyclic Dependencies:
 *        - If there is a circular reference (e.g., A1 depends on A2 and A2
 * depends on A1), detect it and throw errors.
 *
 * Example:
 *    spreadsheet.SetCell('A1', '1');
 *    spreadsheet.SetCell('A2', '2');
 *    spreadsheet.SetCell('A3', '=A1+A2');
 *    spreadsheet.SetCell('A2', '4'); // A3 automatically updates to 5.
 *    spreadsheet.SetCell('A4', '=A3+A2');
 *    spreadsheet.SetCell('A5', '=A3+A4');
 *    spreadsheet.SetCell('B1', '=A1+A2+A3+A4+A5');
 *
 * Follow-up:
 *    If multiple requests are concurrently sent to the spreadsheet, how would
 * you optimize for thread safety and more efficient lookups?
 */
class Excel {
public:
  using Val = std::variant<int, vector<string>>;
  Excel() = default;
  void SetCell(string cell, string val) {
    if (!to_graph_.count(cell)) {
      try {
        int num = std::stoi(val);
        to_graph_.insert({cell, num});
      } catch (...) {
        vector<string> to_cells = Extract(val);
        to_graph_.insert({cell, to_cells});
      }
    }
  }

  int GetCell(const string &cell) {
    // DFS
    // base case
    auto it = to_graph_.find(cell);
    if (it == to_graph_.end()) {
      throw std::runtime_error("INVALID CELL");
    }
    if (std::holds_alternative<int>(it->second)) {
      return std::get<int>(it->second);
    }
    // recursive step
    int sum = 0;
    for (auto &to : std::get<vector<string>>(it->second)) {
      sum += GetCell(to);
    }
    return sum;
  }

  // SetCell2("A3", "=A1+A2"): from -> to: A3 -> A1, A3 -> A2
  void SetCell2(string cell, string val) {
    std::unique_lock lock(mtx_); // Exclusive writes
    int num = 0;
    int old_num = vals_.count(cell) ? vals_[cell] : 0;
    // cout << "old_num = " << old_num << "\n";
    try {
      num = std::stoi(val);
      // cout << "num = " << num << "\n";
      // insert: No overwrite. returns pair<iterator, bool>
      // insert_or_assign or [] can overwrite.
      // to_map_.insert({cell, unordered_set<string>()});
      to_map_.insert_or_assign(cell, unordered_set<string>());
    } catch (...) {
      vector<string> new_to_list = Extract(val);
      unordered_set<string> old_to_list =
          to_map_.count(cell) ? to_map_.at(cell) : unordered_set<string>();
      to_map_[cell] =
          unordered_set<string>(new_to_list.begin(), new_to_list.end());
      unordered_set<string> visited;
      if (IsCyclic(cell, visited, num)) {
        to_map_[cell] = old_to_list; // revert to cells.
        throw std::runtime_error("Cyclic Deps.");
      } else {
        // update from cells
        for (auto &to : new_to_list) {
          auto it = from_map_.find(to);
          if (it == from_map_.end()) {
            from_map_.insert({to, {cell}});
          } else {
            it->second.insert(cell);
          }
        }
      }
    }

    // for (auto &[k, v] : vals_) {
    //   cout << "k = " << k << ", v = " << v << "\n";
    // }
    // for (auto &[k, v] : to_map_) {
    //   cout << "to cells = " << k << ": ";
    //   for (auto& to : v) {
    //     cout << to;
    //   }
    //   cout << "\n";
    // }
    // for (auto &[k, v] : from_map_) {
    //   cout << "from cells = " << k << ": ";
    //   for (auto& from : v) {
    //     cout << from;
    //   }
    //   cout << "\n";
    // }

    // BFS: propagate changes to "from" cells
    int delta = num - old_num;
    deque<string> que;
    que.push_back(cell);
    while (que.size() > 0) {
      size_t lvl_size = que.size();
      for (size_t i = 0; i < lvl_size; ++i) {
        auto q_it = que.begin();
        que.pop_front();
        vals_[*q_it] += delta;
        // cout << "Add " << *q_it << " by " << delta << "\n";
        for (auto &from : from_map_[*q_it]) {
          que.push_back(from);
        }
      }
    }
  }

  // DFS + backtracking (don't forget to erase steps)
  bool IsCyclic(string cell, unordered_set<string> &visited, int &num) {
    visited.insert(cell);
    auto it = to_map_.find(cell);
    if (it == to_map_.end() || it->second.empty()) {
      num += vals_.count(cell) ? vals_.at(cell) : 0;
      visited.erase(cell); // erase step for backtracking.
      return false;
    }

    for (auto &to : it->second) {
      if (visited.contains(to)) {
        return true;
      }
      if (IsCyclic(to, visited, num)) {
        return true;
      }
    }
    visited.erase(cell); // erase step for backtracking.
    return false;
  }

  // O(1)
  int GetCell2(const string &cell) {
    std::shared_lock lock(mtx_); // Concurrent reads
    auto it = vals_.find(cell);
    if (it == vals_.end()) {
      return 0;
    }
    return it->second;
  }

  vector<string> Extract(string &val) {
    int idx = val.find("=");
    istringstream iss(val.substr(idx + 1));
    string token;
    char split = '+';
    vector<string> ret;
    while (std::getline(iss, token, split)) {
      if (!token.empty()) {
        ret.push_back(token);
      }
    }
    return ret;
  }

  std::shared_mutex mtx_; // Pessimistic Two-phase lock (2PL)
  unordered_map<string, Val> to_graph_;
  unordered_map<string, unordered_set<string>> to_map_;
  unordered_map<string, int> vals_;
  unordered_map<string, unordered_set<string>> from_map_;
};

int main() {
  Excel ex;
  cout << "\n======  Basic cell deps ======\n";
  ex.SetCell2("A1", "1");
  ex.SetCell2("A2", "2");
  ex.SetCell2("A3", "=A1+A2");
  ex.SetCell2("A4", "=A2");
  ex.SetCell2("A5", "=A4+A3");
  cout << "A1 = " << ex.GetCell2("A1") << " [EXPECTED: 1]\n";
  cout << "A2 = " << ex.GetCell2("A2") << " [EXPECTED: 2]\n";
  cout << "A3 = " << ex.GetCell2("A3") << " [EXPECTED: 3]\n";
  cout << "A4 = " << ex.GetCell2("A4") << " [EXPECTED: 2]\n";
  cout << "A5 = " << ex.GetCell2("A5") << " [EXPECTED: 5]\n";

  cout << "\n======  Isolate cell ======\n";
  ex.SetCell2("Z99", "99");
  cout << "Z99 = " << ex.GetCell2("Z99") << " [EXPECTED: 99]\n";

  cout << "\n======  Undefined cell ======\n";
  cout << "X0 = " << ex.GetCell2("X0") << " [EXPECTED: 0]\n";

  cout << "\n======  Change nums of a leaf cell, update deps ======\n";
  ex.SetCell2("A1", "7");
  cout << "A1 = " << ex.GetCell2("A1") << " [EXPECTED: 7]\n";
  cout << "A2 = " << ex.GetCell2("A2") << " [EXPECTED: 2]\n";
  cout << "A3 = " << ex.GetCell2("A3") << " [EXPECTED: 9]\n";
  cout << "A4 = " << ex.GetCell2("A4") << " [EXPECTED: 2]\n";
  cout << "A5 = " << ex.GetCell2("A5") << " [EXPECTED: 11]\n";

  cout << "\n======  Change a intermediate cell from deps to num ======\n";
  ex.SetCell2("A3", "100");
  cout << "A1 = " << ex.GetCell2("A1") << " [EXPECTED: 7]\n";
  cout << "A2 = " << ex.GetCell2("A2") << " [EXPECTED: 2]\n";
  cout << "A3 = " << ex.GetCell2("A3") << " [EXPECTED: 100]\n";
  cout << "A4 = " << ex.GetCell2("A4") << " [EXPECTED: 2]\n";
  cout << "A5 = " << ex.GetCell2("A5") << " [EXPECTED: 102]\n";

  cout << "\n======  Change a cell from num to deps ======\n";
  ex.SetCell2("C1", "8");
  ex.SetCell2("C2", "7");
  ex.SetCell2("A1", "=C1+C2");
  ex.SetCell2("A3", "=A1");
  ex.SetCell2("A4", "=A1+A2");
  cout << "A1 = " << ex.GetCell2("A1") << " [EXPECTED: 15]\n";
  cout << "A2 = " << ex.GetCell2("A2") << " [EXPECTED: 2]\n";
  cout << "A3 = " << ex.GetCell2("A3") << " [EXPECTED: 15]\n";
  cout << "A4 = " << ex.GetCell2("A4") << " [EXPECTED: 17]\n";
  cout << "A5 = " << ex.GetCell2("A5") << " [EXPECTED: 32]\n";

  cout << "\n======  Cyclic dep detection ======\n";
  try {
    ex.SetCell2("A2", "=A5+B1");
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << " [EXPECTED: 'Cyclic Deps' error]\n";
  }

  cout << "\n======  Self cycle ======\n";
  try {
    ex.SetCell2("D1", "=D1");
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << " [EXPECTED: 'Cyclic Deps' error]\n";
  }

  cout << "\n======  Concurrency: Deeply nest and large chain update ======\n";
  vector<thread> write_threads;
  write_threads.emplace_back([&ex]() { ex.SetCell2("E1", "1"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E2", "2"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E4", "=E3"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E6", "6"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E5", "=E4+E6"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E3", "=E1+E2"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E7", "=E5"); });

  for (auto &t : write_threads) {
    t.join();
  }

  write_threads.clear();
  write_threads.emplace_back([&ex]() { ex.SetCell2("E1", "3"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E2", "4"); });
  write_threads.emplace_back([&ex]() { ex.SetCell2("E6", "9"); });

  for (auto &t : write_threads) {
    t.join();
  }

  vector<thread> read_threads;
  read_threads.emplace_back([&ex]() {
    cout << "E1 = " << ex.GetCell2("E1") << " [EXPECTED: 3]\n";
    cout << "E2 = " << ex.GetCell2("E2") << " [EXPECTED: 4]\n";
  });
  read_threads.emplace_back([&ex]() {
    cout << "E3 = " << ex.GetCell2("E3") << " [EXPECTED: 7]\n";
    cout << "E4 = " << ex.GetCell2("E4") << " [EXPECTED: 7]\n";
    cout << "E5 = " << ex.GetCell2("E5") << " [EXPECTED: 16]\n";
  });
  read_threads.emplace_back([&ex]() {
    cout << "E6 = " << ex.GetCell2("E6") << " [EXPECTED: 9]\n";
    cout << "E7 = " << ex.GetCell2("E7") << " [EXPECTED: 16]\n";
  });

  for (auto& t : read_threads) {
    t.join();
  }
  return 0;
}
