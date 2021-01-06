#include <iostream>
#include <thread>

void threadFunction1()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    std::cout << "Finished work in thread 1\n"; 
}

void threadFunction2()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // simulate work
    std::cout << "Finished work in thread 2\n"; 

    std::thread t1(threadFunction1);
    t1.join(); // t2 called by main waits for t1 here. 
}

int main()
{
    // create thread
    std::thread t2(threadFunction2);
    
    // main thread waits for the thread t to finish
    t2.join();

    // detach thread t and continue with main
    // a detached thread can't be joined ever again.
    // t.detach();

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work in main\n";
    return 0;
}