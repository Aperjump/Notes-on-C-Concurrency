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

You can use `std::async`to start an **asynchronous** task for which you don't need right away. Rather than giving you back a `std::thread` object to wait on, `std::async` returns a `std::future` object, which will eventually hold the return value of the function.

    auto f6 = std::async(std::launch::async, Y(), 1.2);
    // The function runs on its own thread
    auto f7 = std::async(std::launch::deferred, baz, std::ref(x));
    //function call is deferred until either `wait()` or `get()` is called on future
    auto f8 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x));
