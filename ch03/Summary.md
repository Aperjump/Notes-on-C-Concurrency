## Chapter 3 Sharing data between threads
> In concurrency, a race condition is anything where the outcome depends on the relative ordering of execution of operations on two or more threads; the threads race to perform their respective operations.

Race conditions often occurs when a procedure needs to access two separate data block. To prevent race conditions, we use **mutex**. Before accessing the shared data, you lock the mutex associated with it. When finish the operation, you unlock the current mutex.
Mutex can have problems:
- granularity
- interface
- deadlock

`<mutex>` includes major three parts:

Mutexs:
- `mutex`
- `recursive_mutex`
- `timed_mutex`
- `recursive_timed_mutex`

Locks:
- `lock_guard`
- `unique_lock`

Functions:
- `try_lock`
- `lock`
- `call_once`

`recursive_mutex` can lock the `mutex` object from a thread that is already locking it, acquiring a new level of ownership over the mutex object : the mutex object will actually remain locked owning the thread until its member unlock is called as many times as this level of ownership.

How to use `mutex`? -> group protected data and mutex in a `class`

### Deadlock problem
Each lock is waiting for the other lock to free access. One common advice for avoiding deadlock is to always lock the two mutex in the same order. C++ has a cure for deadlock, `std::lock`, which can lock two or more mutexes at once without risk of deadlock. Code example:

    swap(X& lhs, X& rhs)
    {
      if (&lhs == &rhs)
        return;
      std::lock(lhs.m, rhs.m);
      std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
      std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
      swap(lhs.data, rhs.data);
    }
The `std::adopt_lock` parameter means to the `std::lock_guard` objects that the mutexs are already locked, and they should just adopt the ownership of the existing lock on the mutex rather than attempt to lock the mutex in the constructor.
The author provide many useful advice to avoid **deadlock** problem. Guidelines : **don't wait for another thread if there is a chance its waiting for you.**

- Don't aquire lock if you already hold One
- Avoid calling user-supplied code while holding a lock
- Acquire lock in a fixed order
