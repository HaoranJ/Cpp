#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle(int id) : _id(id) {}
    void operator()()
    {
        std::cout << "Vehicle #" << _id << " has been created" << std::endl;
    }

private:
    int _id;
};

int main() {
  // std::thread t(Vehicle()); // C++'s most vexing parse
  // std::thread t1( (Vehicle(1)) ); // Add an extra pair of parentheses
  std::thread t2 = std::thread( Vehicle(1) ); // copy initialization
  // std::thread t3{ Vehicle(1) }; //uniform initialization with braces

  std::cout << "Finished work in main \n";
  // t1.join();
  t2.join();
  // t3.join();
  return 0;
}