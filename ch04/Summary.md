## Chapter 4 Synchronizing Concurrent Operations
This chapter discuss the use of `<condition_variable>` and `<future>`.

### 1. `condition_variable`
A condition_variable is always associated with some event or other `condition`, and one or more threads can **wait** for that condition to be satisfied. When the condition is satisfied, it can then **notify** other threads waiting on condition.

General paradiam for `condition_variable`:
The thread tend to modify the variable:
- acquire a std::mutex
- perform modification while the lock is held
- execute `notify_one` or `notify_all` on the `std::condition_variable`

The thread which intends to waiti on `condition_variable`:
- acquire a `std::unique_lock<std::mutex>`, on the same mutex as used to protect the shared variable
- execute `wait`, `wait_for`, `wait_until`. The wait operations atomically release the mutex and suspend the execution of the thread.
- when the condition variable is notified, a timeout expires, or a **spurious wakeup** occurs, the thread is awakened, and mutex is atomatically reacquired. The thread should then check the condition and resume waiting if the wake up was spurious.

`<condition_variable>` header has two implementations: `std::condition_variable`, `std::condition_variable_any`.
`condition_variable` acquire `mutex`.
`condition_variable_any` acquire mutex-like data.
I've written a test file in `Listing4.1.cpp`. Notice the use of `wait` method in `condition_variable`. If the condition is not satisfied, `wait()` unlocks the mutex and puts the thread in a blocked or waiting time. When the condition variable is notified by a call to `notify_one()` from the data_preparation_thread, the thread wakes from its slumber, reacquires the lock on the mutex. If the condition hasn't been satisfied, the thread unlocks the mutex and resumes waiting. **Use `unique_lock` rather than `lock_guard`**. The waiting thread must unlock the mutex while its waiting and lock it again afterward, and `lock_guard` does not provide the flexibility.

Member functions in `condition_variable`:
- `notify_one`
- `notify_all`

- `wait`
- `wait_for`
- `wait_until`

signature of `wait`
`void wait(std::unique_lock<std::mutex>& lock)`
`void wait(std::unique_lock<std::mutex>& lock, predicate)`
predicate will return `false`, if the waiting should be continued.

When the waiting thread reacquires the mutex and checks the condition, if it isn't in direct response to a notification from another thread, it's called **spurious wake**.

### 2. futures
If a thread needs to wait for a specific one-off event, it somehow obtains a future representing this event. The thread can then periodically wait on the future for short period of time to see if the event has occurred while performing some other task in between checks. Alternatively, it can do another task until it needs the event to have happened before it can proceed and then just wait for the future to become ready.

`<future>` has two class templates: unique future(`std::future`), and shared_future(`std::shared_future`). In `shared_future`, all instances will become ready at the same time, they may all access any data associated with the event. Althugh futures are used to communicate between threads, the future object themselves don't provide synchronized accesses. If multiple threads need to access a single future object, they must protect access via a mutex or other synchronizaion mechanism.

You can use `std::async`to start an **asynchronous** task for which you don't need right away. Rather than giving you back a `std::thread` object to wait on, `std::async` returns a `std::future` object, which will eventually hold the return value of the function. If the arguments are `rvalue`, the copies are created by `moving` the originals.

    auto f6 = std::async(std::launch::async, Y(), 1.2);
    // The function runs on its own thread
    auto f7 = std::async(std::launch::deferred, baz, std::ref(x));
    //function call is deferred until either `wait()` or `get()` is called on future
    auto f8 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x));

### `package_task`
`package_task<>` ties a future to a function or callable object. When the `std::package_task<>` object is invoked, it calls the associated function or callable object and makes the future ready, with the return value stored as the associated data. If a large operation can be divided into self-contained sub-tasks, each of these can be wrapped in a `std::package_task<>` instance, and the that instance passed to the scheduler or thread pool. This abstracts out the details of the tasks; the scheduler just deal with `std::package_task<>` instances rather than individual functions.

The return type of the specified function signature identifies the type of `std::future<>` returned from the `get_future()` member function, whereas the argument list of the function signature is used to specify the signature of the packaged task's function call operator.

`std::promise<T>` provides a means of a setting a value, which can later be read through an associated `future<T>` object.
The class template `std::promise` provides a facility to store a value or an exception that is later acquired asynchronously via `std::future` object created by `std::promise` object. Each promise is associated with a share state, which contains some state information and a result which may be not yet evaluated.
Exception handling:

    extern std::promise<double> some_promise;
    try
    {
      some_promise.set_value(calculate_value());
    }
    catch(...)
    {
      some_promise.set_exception(std::current_exception());
    }
`std::future` models unique ownership of the asynchronous result. Only one thread can retrive the value, because after the first call to get() there is no value left.
`std::shared_future` instances are **copyable**, so you can have multiple objects referring the same associated state.

    std::promise<int> p;
    std::future<int> f(p.get_future());
    std::shared_future<int> sf(move(f));

    std::promise<string> p;
    std::shared_future<string> sf(p.get_future());


## wait
`condition_variable` has two overloads of the `wait_for()` member function and two overloads of the `wait_until` member functions.  Clocks are reserved in `<chrono>` library.
### Durations
`std::chrono::duration<>` The first template parameter is the type of the representation, and the second is a fraction specifying how many seconds each unit of the duration represents. `chrono::duration<short, ratio<1,60>>`->minutes

    chrono::milliseconds ms(54802);
    chrono::seconds s = chrono::duration_cast<chrono::seconds>(ms);

    future<int> f = async(some_task);
    if (f.wait_for(chrono::milliseconds(35)) == future_status::ready)
      do_some_thing_with(f.get());
The function returns `std::future_status::timeout` or `std::future_status::ready` if future is ready, or `future_status_deferred` if the future's task is deferred.
### Time points
`chrono::time_point<>`
The value of a time point is the length of time(in multiple of the specified duration) since a specific point in time called the epoch of the clock.
`chrono::time_point<chrono::system_clock, chrono::minutes>`

    condition_variable cv;
    bool done;
    mutex m;
    bool wait_loop()
    {
      auto const timeout = std::chrono::steady_clock()::now() +
        chrono::miliseconds(500);
      unique_lock<mutex> lk(m);
      while(!done)
      {
        if (cv.wait_until(lk, timeout) == cv_status::timeout)
          break;
      }
      return done;
    }
`timed_mutex` and `recursive_timed_mutex` support timeouts on locking, both types support `try_lock_for()` and `try_lock_until()` member functions that try to obtain the lock within a specified time period or before a specified time point.

## Using synchronization of operations to simplify
