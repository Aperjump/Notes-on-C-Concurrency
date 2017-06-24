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

## 1. Atomic operations
An atomic operation is an indivisible operation. No half-done state.
The standard **atomic types** can be found in `<atomic>`, and all operations on such types are atomic. The standard `<atomic>` types uses mutex: they (almost) all have an `x.is_lock_free()` function, which allows user to determine whether operations on a given type are done directly with atomic instructions(true), or by using a lock internal(false).
The standard atomic types are not copyable or assignable in the conventional sense.
The `std::atomic<>` class templates isn't just a set of specializations. It does have a primary template that can be used to create an atomic variant of a user-defined type. But the operators are limited to `load()`, `store()`, `exchange()`, `compare_exchange_weak()` and `compare_exchange_strong()`

Atomic operations are divided into three categories:
- Store operations : `memory_order_relaxed`, `memory_order_release`, `memory_order_seq_cst`
- Load operations : `memory_order_relaxed`, `memory_order_consume`, `memory_order_acquire`, `memory_order_seq_cst`
- Read-modify-write operations : `memory_order_relaxed`, `memory_order_consume`, `memory_order_release`, `memory_order_acq_rel`, `memory_order_seq_cst`

The default order is `memory_order_seq_cst`.
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

### `std::atomic<bool>`
You can construct it from a nonatomic `bool`:
`std::atomic<bool> b(true);`
`b = false;`
The assignment operator from a nonatomic `bool` is that it differs from the general convention of returnning a reference to the object it's assigned to: it returns a `bool` with the value assigned instead.
- `store()` : modify contained value
- `exchange()` : access and modify contained value
- `load` : read contained value
- `compare_exchange_weak()`
- `compare_exchange_strong`

We have `compare_exchange_weak(T& expected, T val,
           memory_order sync = memory_order_seq_cst)` and `compare_exchange_strong(T& expected, T val,
           memory_order sync = memory_order_seq_cst)`
They compares the value of atomic variable with a supplied expected value and stores the supplied desired value if they're equal. If the values aren't equal, the expected value is updated with the actual value of atomic variable.
`compare_exchange_weak()` may not be successful even if the original value was equal to the expected value, in which case the value of the variable is unchanged and the return value of `compare_exchange_weak()` is `false`. ---> **machine lacks a single compare-and-exchange instruction**.
This is called **spurious failure**, and can be tested through :

    bool expected = false;
    extern atomic<bool> b;
    while(!b.compare_exchange_weak(expected, true) && !expected);
`compare_exchange_strong`, on the other hand, is guranteed to return false only if the actual value wasn't equal to the expected value.
**Each time through the loop, `expected` is reloaded**.

`std::atomic<T*>: pointer arithmetic`
Besides normal operations, `std::atomic<T*>` also support `fetch_add` and `fetch_sub`

### `atomic<>` primary class template
In order to use `atomic<UDT>` for user-defined class `UDT`, this type must have a trivial copy-assignment operator. The type mustn't have any virtual functions or virtual base classes and must use the compiler-generated copy-assignment operator. Every base classes and non-static data member of a user-defined type must also have a trivial copy-assignment operator. (**permit `memcpy()`** operation)
Besides, user-defined type must also be **bitwise equality comparable**.


## 2. Synchronizing Operations and Enforcing Ordering
### Synchronize-with relationship
How to achieve synchronize-with relationship?
- data structure contain atomic types
- perform appropriate atomic operation.

In C++ standard:
>An atomic operation A that performs a release operation on an atomic object M synchronizes with an atomic operation B that performs an acquire operation on M and takes its value from any side effect in the release sequence headed by A.

If thread A stores a value and thread B reads that value, there's a synchronizes-with relationship between the store in thread A and load in thread B.

### Happens-before relationship
**Happens-before** specifies which operation see the effects of which other operations. Happens-before relationship follows transitive law, If operations in thread A happens-before operations in thread B, B to C, then A to C.

### Memory ordering
In `memory_order` enum. 6 ordering options represent three models: **sequentially consistent ordering**, **acquire_release ordering** and **relaxed ordering**.

**sequentially consistent** means that if all operations on instances of atomic types are sequentially consistent, the behavior of a multi-threaded program is as if all these operations were performed in some particular sequence by a single thread. A sequentially consistent synchronizes-with a sequentially consistent load of the same variable that reads the value stored. On a weakly ordered machine with many processors, it can impose a noticeable performance penalty, because the overall sequence of operations must be kept consistent between the processors, possibly requiring extensive synchronization operations between processors.

**Non-sequentially consistent memory operation**
