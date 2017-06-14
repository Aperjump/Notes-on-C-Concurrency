/***
thread_safe_que
@C++ Concurrency in Action
***/
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
using namespace std;
template<typename T>
class thread_safe_que
{
public:
  thread_safe_que()
  {

  }
  thread_safe_que(const thread_safe_que& other)
  {
    lock_guard<mutex> lk(other.mut);
    data_que = other.data_que;
  }
  thread_safe_que& operator=(
    const thread_safe_que&) = delete;
  void push(T new_value)
  {
    lock_guard<mutex> lk(mut);
    data_que.push(new_value);
    data_cond.notify_one();
  }
  /***
  bool try_pop(T& new_value) will store the retrived value
  in the referenced variable, so it can use the return value
  for status
  ***/
  bool try_pop(T& new_value)
  {
    lock_guard<mutex> lk(mut);
    if (data_que.empty())
      return false;
    value = data_que.front();
    data_que.pop();
    return true;
  }
  std::shared_ptr<T> try_pop()
  {
    lock_guard<mutex> lk(mut);
    if (data_que.empty())
      return shared_ptr<T>>();
    shared_ptr<T> res(make_shared<T>(data_que.front()));
    data_que.pop();
    return res;
  }

  void wait_and_pop(T& value)
  {
    unique_lock<mutex> lk(mut);
    data_cond.wait(lk, [this]{return !data_que.empty();});
    value = data_que.front();
    data_que.pop();
  }
  std::shared_ptr<T> wait_and_pop()
  {
    unique_lock<mutex> lk(mut);
    data_cond.wait(lk,[this]{return !data_que.empty();});
    shared_ptr<T> res(make_shared<T>(data_que.front()));
    data_que.pop();
    return res;
  }
  bool empty() const
  {
      lock_guard<mutex> lk(mut);
      return data_que.empty();
  }
private:
  mutex mut;
  queue<T> data_que;
  condition_variable data_cond;
}
