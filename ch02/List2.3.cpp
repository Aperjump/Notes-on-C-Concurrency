/***
Code List From C++ Concurrency in Action
***/
#include <thread>
#include <iostream>
using namespace std;
class thread_guard
{
    std::thread& t;
  public:
    explicit thread_guard(std::thread& t):t(t)
    {}
    ~thread_guard()
    {
      if (t.joinable())
      {
        cout << "Exit subthread" << endl;
        t.join();
      }
      cout << "Exit, Byebye" << endl;
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

struct func
{
  int& i;
  func(int& i):i(i) {}
  void operator()()
  {
    for (unsigned j = 0; j < 1000000; ++j)
    {
      cout << j << endl;
    }
  }
};

int main()
{
  int some_local_state = 0;
  func my_func(some_local_state);
  std::thread t(my_func);
  thread_guard g(t);
  for (unsigned j = 0; j < 1000000; ++j)
  {
    cout << "Main thread" << j << endl;
  }
}
