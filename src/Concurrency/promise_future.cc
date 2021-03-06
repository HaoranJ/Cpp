#include <iostream>
#include <thread>
#include <future>

// rvalue ref
void modifyMsg(std::promise<std::string> &&prms, std::string msg) {
  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
  std::string modifiedMessage = msg + " has been modified"; 
  prms.set_value(modifiedMessage);
}

void divide(std::promise<double> &&promise, double num, double denom) {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  try {
    if (denom == 0) {
      throw std::runtime_error("Divided by zero!");
    } else {
      promise.set_value(num / denom);
    }
  } catch (...) {
    promise.set_exception(std::current_exception());
  }
}

/*
Promise is the sending end of the communication channel between threads;
Future is the receiving end.
Data can only be got once. The channel is single-use.
*/
int main() {
  std::string msgToThread = "Initial Msg";
  std::promise<std::string> prms;
  std::future<std::string> ftr = prms.get_future();

  std::thread t(modifyMsg, std::move(prms), msgToThread);

  // print original message to console
  std::cout << "Original message from main(): " << msgToThread << std::endl;

  auto status = ftr.wait_for(std::chrono::milliseconds(1000));
  // ftr.wait_until
  if (status == std::future_status::ready) {
    std::string msgFromThread = ftr.get();
    // std::string msgAgain = ftr.get() --> throw exception! 
    std::cout << "Modified message from thread(): " << msgFromThread << std::endl;
  } else if (status == std::future_status::timeout || 
  status == std::future_status::deferred) {
    std::cout << "Unavailable" << std::endl;
  }

  // thread barrier
  t.join();

  /* Pass exceptions via promise-future channel */
  std::promise<double> promise1;
  std::future<double> future1 = promise1.get_future();
  std::thread t1(divide, std::move(promise1),1.0, 0.0);
  try
  {
    double result = future1.get();
    std::cout << "Result = " << result << std::endl;
  }
  catch(std::runtime_error e)
  {
    std::cerr << e.what() << '\n';
  }
  
  t1.join();

  return 0;
}