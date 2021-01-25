#include <iostream>
#include <thread>
#include <vector>

int main() {
  std::vector<std::thread> threads;
  for (size_t i = 0; i < 5; i++) {
    // move semantics
    threads.emplace_back([i]() {
      // wait for certain amount of time
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));

      // perform work
      std::cout << "Hello from Worker thread #" << i << std::endl;
    });
  }

  std::cout << "Hello from Main thread #" << std::endl;

  // thread barrier
  for (auto &t : threads) {
    t.join();
  }

  return 0;
}