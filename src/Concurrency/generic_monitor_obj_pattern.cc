#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <algorithm>
#include <deque>

class Vehicle
{
public:
  Vehicle(int id) : _id(id) {}
  int getID() { return _id; }

private:
  int _id;
};

template<class T>
class MessageQueue
{
public:
  MessageQueue() {}

  // consume
  T receive()
  {
    // perform vector modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    // Handle 'spurious wake-ups' - the system will (for no obvious reason) 
    // wake up a thread.
    // while (messages_.empty()) {
    //   _cond.wait(uLock);
    // }
    // wait() will repeatedly check whether the vector is empty to handle
    // spurious wake-ups.
    // enter the wait state and release the lock
    // wait() repeatedly checks messages_.empty() to avoid spurious wake-ups
    _cond.wait(uLock, [this] { return !messages_.empty(); });
    // remove last vector element from queue
    T v = std::move(messages_.back());
    messages_.pop_back();
    return v; // will not be copied due to return value optimization (RVO) in C++
  }

  // produce
  void send(T &&v)
  {
    // simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);
    // add vector to queue
    std::cout << "   T #" << v.getID() << " will be added to the queue" << std::endl;
    messages_.emplace_back(std::move(v));
    _cond.notify_one();
  }

private:
  std::deque<T> messages_; // list of all Ts waiting to enter this intersection
  std::mutex _mutex;
  std::condition_variable _cond;
};

int main()
{
  // create monitor object as a shared pointer to enable access by multiple threads
  std::shared_ptr<MessageQueue<Vehicle>> queue(new MessageQueue<Vehicle>);

  std::cout << "Spawning threads..." << std::endl;
  std::vector<std::future<void>> futures;
  for (int i = 0; i < 10; ++i)
  {
    // create a new Vehicle instance and move it into the queue
    Vehicle v(i);
    futures.emplace_back(std::async(std::launch::async, &MessageQueue<Vehicle>::send, queue, std::move(v)));
  }

  std::cout << "Collecting results..." << std::endl;
  while (true)
  {
    Vehicle v = queue->receive();
    std::cout << "   Vehicle #" << v.getID() << " has been removed from the queue" << std::endl;
  }

  std::for_each(futures.begin(), futures.end(), [](std::future<void> &ftr) {
    ftr.wait();
  });

  std::cout << "Finished processing queue" << std::endl;

  return 0;
}