#include <iostream>
#include <thread>
#include <string>

void printName(std::string name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    std::cout << "Name (from Thread) = " << name << std::endl;
}

void printName_Ref(std::string &name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    std::cout << "Name (from Thread) = " << name << std::endl;
}

int main() {
  std::string name1 = "MyThread1";
  std::string name2 = "MyThread2";

  std::thread t1(printName, name1, 50); //copy by value
  std::thread t2(printName, std::move(name2), 100); //move semantics

  t1.join(); 
  t2.join();

  std::cout << "Name (from Main) = " << name1 << std::endl;
  std::cout << "Name (from Main) = " << name2 << std::endl;

  std::string name("MyThread");
  // pass by ref. main and t3 thread shares mutable data between them
  std::thread t3(printName_Ref, std::ref(name), 50);
  t3.join();

  name += " (from Main)";
  std::cout << name << std::endl;

  return 0;
}