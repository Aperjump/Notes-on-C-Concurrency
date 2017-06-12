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
