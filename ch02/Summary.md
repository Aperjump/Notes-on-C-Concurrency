## Chapter 2 Managing Threads
### 2.1 Basic Thread Management
#### [1] Creating Thread
In `ch01`, we have learned the basic operation on thread object through reading its official documents. This Chapter makes some detailed discussion on thread usage.
**Note:** Never pass a temporary function object like `std::thread mythread(do_somework())`. Two solutions:
1. `std::thread mythread((do_somework()))`
2. `std::thread mythread{do_somework()}`

#### [2] Waiting Thread finish
After creating one thread, we also need to manage how this thread ends. New thread stops either before the main thread(No problem) or after main thread finishes. If that thread continues running after main thread exits, some referenced data may not valid, leading to undefined behavior. Two methods:
1. `thread.join()`
2. `thread.detach()`

If we choose to wait for a thread to complete, exceptional circumstances should be our first concern. Calling `join()` is liable to be skipped if an exception is thrown after the thread has been started but before the call to `join()`. Resource Acquisition is Initialization(RAII) can be good solution.

    class thread_guard
    {
    private:
        std::thread& t;
    public:
        ~thread_guard()
        {
          if (t.joinable())
          {
            t.join();
          }
        }
    }
Find examples in `List2.3.cpp`.
If we choose to detach new thread, we have no direct control over that.
> Detached threads(daemon thread) truly run in the background; ownership and control are passed over to the c++ Runtime Library, which ensures that the resources associated with the thread are correctly reclaimed when the thread exits.

#### [3] Passing Arguments
**The default arguments are copied into internal storage**.
To pass a reference we should use `std::ref(data)`            
> If you are familiar with `std::bind`, the parameter-passing semantics will be unsurprising, because both the operation of the `std::thread` constructor and the operation of `std::bind` defined in terms of the same mechanism.

This paragraph is not clear enough. I'd like to write another article explaining their relationships.
When values are **moved** into thread constructor, add `std::move(para)`.
I've written testing code in  `List2.6.cpp`, but that doesn't work.
The `move` support in `std::thread` allows for containers of `thread` objects, if those containers are move aware.

    void do_work(unsigned id);
    void f()
    {
      std::vector<std::thread> threads;
      for (unsigned i = 0; i < 20; i++)
      {
        threads.push_back(std::thread(do_work,i));
      }

      std::for_each(threads.begin(), threads.end(); std::mem_fn(&std::thread::join));
    }
