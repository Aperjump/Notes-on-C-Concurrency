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
