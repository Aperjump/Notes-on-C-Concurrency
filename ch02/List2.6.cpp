/***
Scoped_thread
Help explain the move semantics of thread_guard
@c++ Concurrency in Action
***/
#include <thread>
#include <iostream>
using namespace std;
class Scoped_thread
{
  std::thread t;
public:
  Scoped_thread(std::thread _t):
    t(std::move(_t))
    {
      cout << "Success?" << endl;
      if (!t.joinable())
        throw std::logic_error("No thread");
    }
  ~Scoped_thread()
  {
    t.join();
  }
  Scoped_thread(Scoped_thread const&) = delete;
  Scoped_thread& operator=(Scoped_thread const&) = delete;
};

struct func
{
  int& i;
  func(int& i):i(i) {}
  void operator()()
  {
    for (unsigned j = 0; j < 1000000; j++)
    {
      cout << j << endl;
    }
  }
};

int main()
{
  int some_local_state;
  some_local_state = 1;
  func myfunc(some_local_state);
  Scoped_thread t2{std::thread(myfunc)};
  for (unsigned j = 0; j < 1000000; j++)
  {
    cout << "Main thread " << j << endl;
  }
}
/***
I found there is only one thread runnable in the environment, so I've post the question
in Stackoverflow.


***/
