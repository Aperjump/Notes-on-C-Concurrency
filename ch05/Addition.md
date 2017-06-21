### Lockfree programming and `<atomic>`
`<atomic>` header in C++ is the cornerstone for  **lock-free programming**. On all modern processors, reading and writing native types are atomic, as long as the memory bus is as long as the type being read or wrriten. On x86 and x64 CPU,  there is no guarantee that reads and writes larger than eight bytes are atomic. If you are doing complex instruct, such as Read-modify-write operation, the thread can be swaped out.
[Lockless Programming Considerations for Xbox 360 and Microsoft Windows](https://msdn.microsoft.com/en-us/library/windows/desktop/12a04hfd(v=vs.85).aspx) suggests three ways to achieve atomic operation:
- Naturally atomic operations
- Locks to wrap composite operations
- Operating system functions that implement atomic versions of composite operations

A major part in `<atomic>` header is `memory_order` which deals with **Reordering** problem.
> Reads and writes do not always happen in the order that you have written them in your code, and this can lead to very confusing problems. In many multi-threaded algorithms, a thread writes some data and then writes to a flag that tells other threads that the data is ready. This is known as a write-release. If the writes are reordered, other threads may see that the flag is set before they can see the written data.

The ordering mechanism can be found in the passage. To protect the order of operation, we should gurantee both compiler and CPU ordering of reads and writes. x86 and x64 cannot reorder write relative to write, or read relative to read, they can reorder the order between read and write.
**x86 and x64 CPUs only reorder reads ahead of previous writes if they target different locations. **
