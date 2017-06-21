### Lockfree programming and `<atomic>`
`<atomic>` header in C++ is the cornerstone for  **lock-free programming**. On all modern processors, reading and writing native types are atomic, as long as the memory bus is as long as the type being read or wrriten. On x86 and x64 CPU,  there is no guarantee that reads and writes larger than eight bytes are atomic. If you are doing complex instruct, such as Read-modify-write operation, the thread can be swaped out.
[Lockless Programming Considerations for Xbox 360 and Microsoft Windows](https://msdn.microsoft.com/en-us/library/windows/desktop/12a04hfd(v=vs.85).aspx) suggests three ways to achieve atomic operation:
- Naturally atomic operations
- Locks to wrap composite operations
- Operating system functions that implement atomic versions of composite operations

A major part in `<atomic>` header is `memory_order` which deals with **Reordering** problem.
> Reads and writes do not always happen in the order that you have written them in your code, and this can lead to very confusing problems. In many multi-threaded algorithms, a thread writes some data and then writes to a flag that tells other threads that the data is ready. This is known as a write-release. If the writes are reordered, other threads may see that the flag is set before they can see the written data.

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
