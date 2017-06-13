/***
Hierachy_mutex
@c++ Concurrency in Action
***/

#include <mutex>
class Hierachy_mutex
{
  std::mutex internal_mutex;
  unsigned long const hierachy_value;
  unsigned previous_hierachy_value;
  static thread_local unsigned long this_thread_hierachy_value;

  void check_for_hierarchy_violation()
  {
    if (this_thread_hierachy_value <= hierachy_value)
    {
      throw std::logic_error("mutex hierachy violated");
    }
  }
  void update_hierarchy_value()
  {
    previous_hierachy_value = this_thread_hierachy_value;
    this_thread_hierachy_value = hierachy_value;
  }
public:
  explicit Hierachy_mutex(unsigned long value):
        hierachy_value(value),
        previous_hierachy_value(0)
  {}
  void lock()
  {
    check_for_hierarchy_violation();
    internal_mutex.lock();
    update_hierarchy_value();
  }
  void unlock()
  {
    this_thread_hierachy_value = previous_hierachy_value;
    internal_mutex.unlock();
  }
  bool try_lock()
  {
    check_for_hierarchy_violation();
    if (!internal_mutex.try_lock())
        return false;
    update_hierarchy_value();
    return true;
  }
};
