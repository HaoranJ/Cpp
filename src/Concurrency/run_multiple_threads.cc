#include <iostream>
#include <thread>
#include <vector>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {
      std::cout << "init by thread " << std::this_thread::get_id() << std::endl;
    }
    int getID() { return _id; }

private:
    int _id;
};

class Node {
  public:
  Vehicle* v_;
};

void SetUp(Node* node) {
  std::cout << "SetUp by thread " << std::this_thread::get_id() << std::endl;
  static Vehicle* v = new Vehicle(0);
  node->v_ = v;
}

int main() {
  std::cout << "Hello from Main thread #" << std::endl;
  std::vector<std::thread> threads;
  for (size_t i = 0; i < 5; i++) {
    Node* node = new Node();
    std::thread t(SetUp, node);
    threads.push_back(std::move(t));
    // // move semantics
    // threads.emplace_back([i]() {
    //   // wait for certain amount of time
    //   std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));

    //   // perform work
    //   std::cout << "Hello from Worker thread #" << i << std::endl;
    // });
  }

  // std::cout << "Hello from Main thread #" << std::endl;

  // thread barrier
  for (auto &t : threads) {
    t.join();
  }

  return 0;
}