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

### Compiler reordering
Since the compiler think your program is single-thread and and assume it thread-safe, the responsibility falls on programmers. Therefore, you need to tell the compiler when it is not allowed to reorder reads and writes.






### Sequential Consistency
Sequential consistency means that all threads agree on the order in which memory operations occured, and that order is consistent with the order of operations in the program source code.
Using C++ `atomic` types can achieve this, but the compiler outputs additional instructions behind the scenes, typically memory fences or RMW operations.
### Memory Reordering
If your environment does not support **sequential consistency**, the only cure is to prevent **memory reordering** by prohibit **CPU reordering** and **compiler reordering**:
- A lightweight sync or fence instruction
- A full memory fence instruction
- acquire or release semantics
