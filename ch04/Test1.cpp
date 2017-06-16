// Copyright
// Author : Aperion_wang
// Email : wangwei_aperion@163.com
#include <iostream>       // std::cout
#include <functional>     // std::ref
#include <thread>         // std::thread
#include <future>         // std::promise, std::future

void print_int(std::future<int>& fut) {
	int x = fut.get(); // get share state
	std::cout << "value: " << x << '\n'; // 打印 value: 10.
}

int main()
{
	std::promise<int> prom;
	std::future<int> fut = prom.get_future();
	std::thread t(print_int, std::ref(fut));
	prom.set_value(10);
	t.join();
	return 0;
}
