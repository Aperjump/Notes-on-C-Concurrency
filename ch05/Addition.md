### Lockfree programming and `<atomic>`
`<atomic>` header in C++ is the cornerstone for  **lock-free programming**. On all modern processors, reading and writing native types are atomic, as long as the memory bus is as long as the type being read or wrriten. On x86 and x64 CPU,  there is no guarantee that reads and writes larger than eight bytes are atomic. If you are doing complex instruct, such as Read-modify-write operation, the thread can be swaped out.
[Lockless Programming Considerations for Xbox 360 and Microsoft Windows](https://msdn.microsoft.com/en-us/library/windows/desktop/12a04hfd(v=vs.85).aspx) suggests three ways to achieve atomic operation:
- Naturally atomic operations
- Locks to wrap composite operations
- Operating system functions that implement atomic versions of composite operations

A major part in `<atomic>` header is `memory_order` which deals with **Reordering** problem.
> Reads and writes do not always happen in the order that you have written them in your code, and this can lead to very confusing problems. In many multi-threaded algorithms, a thread writes some data and then writes to a flag that tells other threads that the data is ready. This is known as a write-release. If the writes are reordered, other threads may see that the flag is set before they can see the written data.

**Read-modify-write(RMW)** operations allow you to perform more complex transactions atomically.It will read a memory location and write a new value into it simultaneously, either with a completely new value or some function of the previous value, like `std::atomic<int>::fetch_add` in C++11. But as the book said, you have to test whether the operation is atomic on the machine using `std::atomic<>::is_lock_free`

**Compare-and-swap(CAS)** compares the contents of a memory location to a given value and, only if they are the same, modifies the contents of that memory location to a new given value. The result of the operation must indicate whether it performed the substitution; this can be done either with a boolean responce or by returning the value read from the memory location.
Algorithms built around CAS typically read some key memory location and remember the old value. Based on that old value, they compute some new value. Then they try to swap in the new value using CAS, where the comparison checks for the location still being equal to the old value. If CAS indicates that the attempt has failed, it has to be repeated from the beginning: the location is re-read, a new value is re-computed and the CAS is tried again.Look [website](https://en.wikipedia.org/wiki/Compare-and-swap)

### CPU reordering

The ordering mechanism can be found in the passage. To protect the order of operation, we should gurantee both compiler and CPU ordering of reads and writes. x86 and x64 cannot reorder write relative to write, or read relative to read, they can reorder the order between read and write.
**x86 and x64 CPUs only reorder reads ahead of previous writes if they target different locations. **

The main constructs used to prevent reordering of reads and writes are called **read-acquire** and **write-release** barriers. read-acquire/write-release is a read/write of a flag or other variable to gain/give away ownership of a resource. coupled with a barrier against reordering.
>A read-acquire executes before all reads and writes by the same thread that follow it in program order.
A write-release executes after all reads and writes by the same thread that precede it in program order.

    // Read that acquires the data.
    if( g_flag )
    {
        // Guarantee that the read of the flag executes before
        // all reads and writes that follow in program order.
        BarrierOfSomeSort();

        // Now we can read and write the shared data.
        int localVariable = sharedData.y;
        sharedData.x = 0;

        // Guarantee that the write to the flag executes after all
        // reads and writes that precede it in program order.
        BarrierOfSomeSort();

        // Write that releases the data.
        g_flag = false;
    }
- Weak Memory Models:

Any load or store operation can effectively be reordered with any other load or store operation, as long as it would never modify the behavior of a single, isolated thread.
- Strong Memory Models:

>A strong hardware memory model is one in which every machine instruction comes implicitly with acquire and release semantics. As a result, when one CPU core performs a sequence of writes, every other CPU core sees those values change in the same order that they were written.

The x86/64 family of processors is usually strongly-ordered.

You can enforece correct memory ordering on the processor by issuing any instruction which acts as a **memory barrier**.  
Examples of instructions which act as memory barriers includes: `load(std::memory_order_acquire)`.
In terms of different memory ordering techinques, I highly recommend this [passage](http://preshing.com/20120710/memory-barriers-are-like-source-control-operations/)
**memory fence instructions** act as memory barrier. Four types: `#LoadLoad`, `#LoadStore`, `#StoreLoad`,`#StoreStore`.
`#StoreLoad` is designed to prevent the reordering of a store followed by a load.
#### `#LoadLoad`
A LoadLoad barrier effectively prevents reordering of loads performed before the barrier with loads performed after the barrier.

    if (IsPublished)                   // Load and check shared flag
    {
        LOADLOAD_FENCE();              // Prevent reordering of loads
        load Value;                  // Load published value
    }
#### `#StoreStore`
A StoreStore barrier effectively prevents reordering of stores performed before the barrier with stores performed after the barrier.

    Value = x;                         // Publish some data
    STORESTORE_FENCE();
    IsPublished = 1;                   // Set shared flag to indicate availability of data

#### `#LoadStore`
A LoadStore barrier ensures all load performed before barrier while store happens after the barrier.  
#### `#StoreLoad`
A StoreLoad barrier ensures that all stores performed before the barrier are visible to other processors, and that all loads performed after the barrier receive the latest value that is visible at the time of the barrier.
### Compiler reordering
Compiler developers follow the rules of protecting behavior of single-threaded program. Thus, it largely unnoticed by programmers writing single-threaded code. It often goes unnoticed in multithreaded programming, too, since mutexes, semaphores and events are all designed to prevent memory reordering around their call sites.
Since the compiler think your program is single-thread and and assume it thread-safe, the responsibility falls on programmers. Therefore, you need to tell the compiler when it is not allowed to reorder reads and writes.
The minimalist approach to preventing compiler reordering is by using a special directive knowns as a **compiler barrier**. `_ReadWriteBarrier` works in MV C++. In C++11  atomic library standard, every non-relaxed atomic operation acts as a compiler barrier as well.

    int Value;
    std::atomic<int> IsPublished(0);

    void sendValue(int x)
    {
        Value = x;
        // <-- reordering is prevented here!
        IsPublished.store(1, std::memory_order_release);
    }
**The majority of function calls act as a compiler barriers.** A call to external function is even stronger than a compiler barrier, since the compiler has no idea what the function's side effects will be.
### Sequential Consistency
Sequential consistency means that all threads agree on the order in which memory operations occured, and that order is consistent with the order of operations in the program source code.
Using C++ `atomic` types can achieve this, but the compiler outputs additional instructions behind the scenes, typically memory fences or RMW operations.
### Memory Reordering
If your environment does not support **sequential consistency**, the only cure is to prevent **memory reordering** by prohibit **CPU reordering** and **compiler reordering**:
- A lightweight sync or fence instruction
- A full memory fence instruction
- acquire or release semantics


### C++ `<atomic>`
**Acquire semantics** is a property that can only apply to operations that read from shared memory, whether they are read-modify-write operations or plain loads. The operation is then considered a read-acquire. Acquire semantics prevent memory reordering of the read-acquire with any read or write operation that follows it in program order.

**Release semantics** is a property that can only apply to operations that write to shared memory, whether they are read-modify-write operations or plain stores. The operation is then considered a write-release. Release semantics prevent memory reordering of the write-release with any read or write operation that precedes it in program order.

- `Read-acquire` : `#LoadLoad`, `#LoadStore`
- `write-release` : `#LoadStore`, `#StoreStore`
