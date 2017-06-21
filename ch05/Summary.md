## Chapter 5 The C++ memory model and Operations on atomic types
Atomic types and operations allow programmers "get close to machine", providing facilities for low-level synchronization operations that will commonly reduce to one or two CPU instructions.
### Memory model
Two aspects: `structural` and `concurrency`
- Every variable is an object, including those that are members of other objects
- Every object occupies at least one memory location
- Varaibles of fundamental type such as `int` or `char` are exactly one memory location, whatever their size.
- Adjacent bit fields are part of the same memory location

Two solutions for race condition:
- mutex
- ordered atomic operation

If there's no enforced ordering between two accesses to a single memory location from separate threads, one or both of those accesses is not atomic , and one or both is write, then this is a data race and causes undefined behavior.

## Atomic operations
An atomic operation is an indivisible operation. No half-done state.
The standard **atomic types** can be found in `<atomic>`, and all operations on such types are atomic. The standard `<atomic>` types uses mutex: they (almost) all have an `x.is_lock_free()` function, which allows user to determine whether operations on a given type are done directly with atomic instructions(true), or by using a lock internal(false).
The standard atomic types are not copyable or assignable in the conventional sense.
The `std::atomic<>` class templates isn't just a set of specializations. It does have a primary template that can be used to create an atomic variant of a user-defined type. But the operators are limited to `load()`, `store()`, `exchange()`, `compare_exchange_weak()` and `compare_exchange_strong()`

Atomic operations are divided into three categories:
- Store operations
- Load operations
- Read-modify-write operations

###  `std::atomic_flag`
`atomic_flag` must be initialized with `ATOMIC_FLAG_INIT`
Once you have your flag object initialized, there are only three things you can do with it: destroy it, clear it, or set it and query the previous value.
- `clear()`
- `test_and_set()`
Both methods can have a memory order specified.


    typedef enum memory_order {
        memory_order_relaxed,   // relaxed
        memory_order_consume,   // consume
        memory_order_acquire,   // acquire
        memory_order_release,   // release
        memory_order_acq_rel,   // acquire/release
        memory_order_seq_cst    // sequentially consistent
    } memory_order;
These `std::memory_order` specifiers are used to specify how other operations on different threads are synchronized.
- memory_order_relaxed
The operation is ordered to happen atomically at some point.
This is the loosest memory order, providing no guarantees on how memory accesses in different threads are ordered with respect to the atomic operation.
- memory_order_consume
[Applies to loading operations]
The operation is ordered to happen once all accesses to memory in the releasing thread that carry a dependency on the releasing operation (and that have visible side effects on the loading thread) have happened.
- memory_order_acquire
[Applies to loading operations]
The operation is ordered to happen once all accesses to memory in the releasing thread (that have visible side effects on the loading thread) have happened.
- memory_order_release
[Applies to storing operations]
The operation is ordered to happen before a consume or acquire operation, serving as a synchronization point for other accesses to memory that may have visible side effects on the loading thread.
- memory_order_acq_rel
[Applies to loading/storing operations]
The operation loads acquiring and stores releasing (as defined above for memory_order_acquire and memory_order_release).
- memory_order_seq_cst
The operation is ordered in a sequentially consistent manner: All operations using this memory order are ordered to happen once all accesses to memory that may have visible side effects on the other threads involved have already happened.This is the strictest memory order, guaranteeing the least unexpected side effects between thread interactions though the non-atomic memory accesses.
For consume and acquire loads, sequentially consistent store operations are considered releasing operations.

Besides, you cannot assign or copy one `atomic_flag` with another variable, since they involve two objetcs. A single action on two objects can't be atomic.


    class spinlock_mutex
    {
      std::atomic_flag flag;
    public:
      spinlock_mutex() :
        flag(ATOMIC_FLAG_INIT)
      {}
      void lock()
      {
        while(flag.test_and_set(std::memory_order_acquire));
      }
      void unlock()
      {
        flag.clear(std::memory_order_release);
      }
    }
