#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

using namespace std;
mutex m;
deque<package_task<void()>> tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()
{
  while(!gui_shutdown_message_received())
  {
    get_and_process_gui_message();
    package_task<void()> task;
    {
      lock_guard<mutex> lk(m);
      if (task.empty())
        continue;
      task = move(task.front());
      tasks.pop_front();
    }
    task();
  }
}

thread gui_bg_thread(gui_thread);
template<typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
  package_task<void()> task(f);
  future<void> res = task.get_future();
  lock_guard<mutex> lk(m);
  tasks.push_back(move(task));
  return res;
}
