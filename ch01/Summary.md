## 1. Basic Introduction
The summary is based on [C++ Concurrency in Action: Practical Multithreading](https://www.amazon.com/C-Concurrency-Action-Practical-Multithreading/dp/1933988770/ref=sr_1_1?ie=UTF8&qid=1497083420&sr=8-1&keywords=c%2B%2B+concurrency+in+action) written by Anthony Williams. I'm still a fresh student in concurrency world. Thus I wish to combine my own learning experience with code from the book to help starters have better understanding of the material. For people taking first step into multi-threading world, it's easy to mess things up. I hope my notes can help those people jump out of the concurrency jungle.I'm currently a student in finance but eager to rush into the world of computer science. And I wish, starting from this project, I can get on my way.

The original work, of course, belongs to Anthony Williams, but I wanna to add some material based on his book. The former code is somewhat high-level, and makes the materials a bit of unaccessible for fresh students.
## 2. A general Introduction to concurrency
From the book:
>At the simplest and most basic level, concurrency is about two or more separate activities happening at the same time.

Concurrency literally means doing several tasks at the same time. In the past, multitasking system relies on **task switching** to achieve the effect, but today, multiple processing unit in one computer enables us to perform real parallism (reference to **hardware concurrency** in the book) on applications.

A system can perform more tasks than the number of processor cores indicates that more than one tasks can perform concurrently on a single CPU. Some mechanism happens between hardware CPU and "operating-level CPU". Virtual CPUs hidden in operating systems can support more tasks running at same time.

## 3. Two approaches to concurrency
### [1] Multiple process
Processes communicate through **interprocess communication channel**. Since one process in operating system is represented as **process control block**(PCB), their interaction can be very expensive.
### [2] Multiple threads
> Threads are much like lightweight processes: each thread runs independently of the others, and each thread may run a different sequence of instructions.

Threads in the same process can share the same address space. This feature facilitates threads communication, but introduce new problem of threads management.
This part is the major discussion of the book.

## 4. Why use concurrency
The author gives us mainly three reasons:
1. good for module separation
2. good for responsiveness
3. good for performance

Creating threads can be expensive, managing threads is also a hard work, Switching threads too often leads to inefficiency.
