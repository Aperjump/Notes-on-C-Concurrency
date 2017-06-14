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
