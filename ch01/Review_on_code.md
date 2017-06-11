My working environment is Ubuntu 16.04, thus compiling the code can be tedious.
I used the following method to alleviate coding:
`alias gcon=g++ -std=c++11 -Wall -pthread`
I've read `<thread>` reference in [Cppreference Website](en.cppreference.com/w/cpp/thread) and find some property in C++ thread.

### 1. Constructor
Methods:
- `thread() noexcept`
- `thread(thread&& other) noexcept`
- `explicit thread(Function&&, Args&&...arg)`
- `thread(const thread&) = delete`

Note that threads are **noncopyable**, and one thread can delegate its control to another thread with `move` semantics.  We used its third constructor. Create new std::thread and associate with a **callable** object, if there any mistakes when copy other parameters, exception will be thrown to the current thread.

### 2. Move equal
- `thread& operator=(thread&& other) noexcept`

### 3. Observers
- `joinable()`
- `get_id()`
- `native_handle()`
- `hardware_concurrency()`

`native_handle()` can be used to communicate between C++ thread and POSIX system, and `hardware_concurrency` returns the number of concurrent threads supported by the implementation.

### 4. Operations
- `join`
- `detach`
- `swap`
