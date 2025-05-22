#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>
using namespace std;

// GPUCredit System
//
// Design a system to manage GPU usage credits with the following functionality:
//
// 1. void AddCredit(string credit_id, int amount, int start_time, int duration)
//    - Adds a credit identified by `credit_id`.
//    - The credit is active during the interval [start_time, start_time +
//    duration).
//    - The credit can be used multiple times within its active window.
//    - Time windows of different credits may overlap.
//
// 2. int GetBalance(int timestamp)
//    - Returns the total available credit balance at the given timestamp.
//    - Only includes credits that are active at the timestamp.
//    - Expired or not-yet-active credits are ignored.
//    - If no credits are available, returns 0.
//
// 3. void UseCredit(int timestamp, int amount)
//    - Attempts to consume up to `amount` credits at the given timestamp.
//    - Credits are consumed in order of earliest expiration (greedy).
//    - If not enough credits are available, uses as much as possible.
//    - Remaining balance for each credit is reduced accordingly.
//    - Balance can never go negative.
//    - A credit is valid during the interval [start_time, start_time + duration],
// inclusive.
//    - This means a credit with duration = 0 is valid **only** at start_time.
//
// Notes:
// - Credit usage is reusable within its validity period.
// - Balance should reflect real-time availability and deduct usage correctly.
// - The system should efficiently expire or clean up stale credits.
class GPUCredit {
public:
  // [10, 50]: +5 -> +3
  // [20, 70]: +10
  // --> ts=30: use -2
  // ts=60: check
  GPUCredit() {}

  void AddCredit(string id, int amount, int start, int duration) {
    auto time = std::make_pair(start, start + duration);
    credit[time] += amount;
  }

  int GetBalance(int time) {
    int bal = 0;
    auto interval = std::make_pair(time, INT32_MAX);
    for (auto it = credit.begin(); it != credit.upper_bound(interval); ++it) {
      auto time_range = it->first;
      if (time_range.first <= time && time <= time_range.second) {
        bal += it->second;
      }
    }
    return bal;
  }

  // For every id, this function will be called at most once at every timestamp.
  void UseCredit(int time, int amount) {
    auto interval = std::make_pair(time, INT32_MAX);
    for (auto it = credit.begin(); it != credit.upper_bound(interval);) {
      auto start = it->first.first, end = it->first.second;
      auto &left_credit = it->second;
      // cout << "start = " << start << ",end = " << end << ", credit = " <<
      // left_credit << "\n";
      if (start <= time && time <= end) {
        if (left_credit > amount) {
          left_credit -= amount;
          amount = 0;
          break;
        } else {
          // cout << "remove!" << "\n";
          amount -= left_credit;
          it = credit.erase(it);
          continue;
        }
      }
      ++it;
    }
    // cout << "map size = " << credit.size() << "\n";
  }

private:
  map<pair<int, int>, int> credit;
};

int main() {
  GPUCredit gpu;

  std::cout << "=== Basic Case ===\n";
  gpu.AddCredit("microsoft", 10, 10, 30);
  std::cout << gpu.GetBalance(0) << " (expected 0)\n";
  std::cout << gpu.GetBalance(10) << " (expected 10)\n";
  std::cout << gpu.GetBalance(40) << " (expected 10)\n";
  std::cout << gpu.GetBalance(41) << " (expected 0)\n";

  std::cout << "\n=== Overlap and Use ===\n";
  GPUCredit gpu2;
  gpu2.AddCredit("amazon", 40, 10, 50); // Active 10–60
  gpu2.UseCredit(30, 30);               // Should consume 30
  std::cout << gpu2.GetBalance(40) << " (expected 10)\n";
  gpu2.AddCredit("google", 20, 60, 10); // Active 60–70
  std::cout << gpu2.GetBalance(60) << " (expected 30)\n";
  std::cout << gpu2.GetBalance(61) << " (expected 20)\n";
  std::cout << gpu2.GetBalance(70) << " (expected 20)\n";
  std::cout << gpu2.GetBalance(71) << " (expected 0)\n";

  std::cout << "\n=== Edge Case: Use More Than Balance ===\n";
  GPUCredit gpu3;
  gpu3.AddCredit("openai", 10, 10, 30);
  std::cout << gpu3.GetBalance(80) << " (expected 0)\n";
  gpu3.UseCredit(10, 100000000); // Should consume 10
  std::cout << gpu3.GetBalance(10) << " (expected 0)\n";
  gpu3.AddCredit("openai", 10, 20, 10);
  std::cout << gpu3.GetBalance(20) << " (expected 10)\n";

  std::cout << "\n=== Edge Case: Zero Duration ===\n";
  GPUCredit gpu4;
  gpu4.AddCredit("temp", 100, 50, 0); // Should be active only at time = 50
  std::cout << gpu4.GetBalance(50) << " (expected 50)\n";

  std::cout << "\n=== Edge Case: Same Expiry ===\n";
  GPUCredit gpu5;
  gpu5.AddCredit("a", 5, 10, 20);  // Expires 30
  gpu5.AddCredit("b", 10, 20, 10); // Also expires 30
  gpu5.UseCredit(25, 12);          // Should use 5 from a, 7 from b
  std::cout << gpu5.GetBalance(25) << " (expected 3)\n";

  std::cout << "\n=== Future Credit ===\n";
  GPUCredit gpu6;
  gpu6.AddCredit("future", 100, 100, 10);
  std::cout << gpu6.GetBalance(99) << " (expected 0)\n";
  std::cout << gpu6.GetBalance(100) << " (expected 100)\n";
  gpu6.UseCredit(100, 50);
  std::cout << gpu6.GetBalance(100) << " (expected 50)\n";

  return 0;
}
