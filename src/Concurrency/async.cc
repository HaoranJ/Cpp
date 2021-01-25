#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <memory>

double divide(double num, double denom) {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  if (denom == 0) {
    throw std::runtime_error("Divided by zero!");
  } else {
    return num / denom;
  }
}

int main() {

  std::future<double> future1 = std::async(divide, 1.0, 0.2);
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