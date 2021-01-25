#include <iostream>
#include <thread>

class Vehicle
{
public:
    Vehicle() : _id(0) {}
    void addID(int id) { _id = id; }
    void createID() { _id = 99; }
    void printID()
    {
        std::cout << "Vehicle ID=" << _id << std::endl;
    }

private:
    int _id;
};

int main() {
  Vehicle v1, v2;
  // v1 is passed by value(copy)
  std::thread t1(&Vehicle::addID, v1, 1);
  // v2 is passed by ref
  std::thread t2(&Vehicle::addID, &v2, 2);
  t1.join();
  t2.join();
  v1.printID();
  v2.printID();

  std::shared_ptr<Vehicle> v(new Vehicle);
  std::thread t3(&Vehicle::createID, v);
  t3.join();
  v->printID();
  return 0;
}