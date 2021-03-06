#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <algorithm>
#include <mutex>
/* Mutex
1. std::mutex, recursive_mutex, timed_mutex;
2. deadlocks have two typical cases:
  i. One thread throws an exception before unlock(), other threads indefinitely waiting for the lock release.
  ii. Two threads are circularly waiting for each other's unlock
*/
class Vehicle
{
public:
  Vehicle(int id) : _id(id) {}

private:
  int _id;
};

class WaitingVehicles
{
public:
  WaitingVehicles() : _tmpVehicles(0) {}

  // getters / setters
  void printSize()
  {
    mutex_.lock();
    std::cout << "#vehicles = " << _tmpVehicles << std::endl;
    mutex_.unlock();
  }

  // typical behaviour methods
  void pushBack(Vehicle &&v)
  {
    // A RAII-based mutex wrapper, then lock_guard goes out of the scope, it's desctructed with mutex unlock();
    std::lock_guard<std::mutex> lock(mutex_);
    //_vehicles.push_back(std::move(v)); // data race would cause an exception
    int oldNum = _tmpVehicles;
    std::this_thread::sleep_for(std::chrono::microseconds(1)); // wait deliberately to expose the data race
    _tmpVehicles = oldNum + 1;
  }

private:
  std::vector<Vehicle> _vehicles; // list of all vehicles waiting to enter this intersection
  int _tmpVehicles;
  std::mutex mutex_;
};

int main()
{
  std::shared_ptr<WaitingVehicles> queue(new WaitingVehicles);
  std::vector<std::future<void>> futures;
  for (int i = 0; i < 1000; ++i)
  {
    Vehicle v(i);
    futures.emplace_back(std::async(std::launch::async, &WaitingVehicles::pushBack, queue, std::move(v)));
  }
  
  std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
    ftr.wait();
  });

  queue->printSize();

  return 0;
}
