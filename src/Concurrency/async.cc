#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <memory>

double divide(double num, double denom) {
  std::cout << "Worker thread id = " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  if (denom == 0) {
    throw std::runtime_error("Divided by zero!");
  } else {
    return num / denom;
  }
}

/* Thread vs Task(std::async())
Tasks: the system takes care of many details like join; 
  will be using with a pool of already created threads (i.e. thread pool);
  throughput - many executions are in parallel;
  keep task simple, primarily use it for promise-future operation.
Threads: overhead to construct/destruct by allocating memory/stack/kernel data;
  latency - when functions can block(e.g. file input, server connection), threads can avoid the program to be blocked when the server is waiting for a response. 
*/
int main() {
  std::cout << "Main thread id = " << std::this_thread::get_id() << std::endl;
  // std::launch::async -> async; deferred -> synchronous
  std::future<double> future1 = std::async(std::launch::deferred ,divide, 1.0, 0.2);
  try
  {
    double result = future1.get();
    std::cout << "Result = " << result << std::endl;
  }
  catch(std::runtime_error e)
  {
    std::cerr << e.what() << '\n';
  }

  return 0;
}