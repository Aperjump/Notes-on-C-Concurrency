// Copyright
// Author : Aperion_wang
// Email : wangwei_aperion@163.com
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <iostream>
#define NUM 100
#define MAXNUM 200
using namespace std;

class ProtectedData
{
public:
	mutex mut;
	queue<double> data_que;
	condition_variable data_cond;
	void init();
};

void ProtectedData::init()
{
	for (int i = 0; i < NUM; i++)
	{
		data_que.push(i);
	}
}

void data_preparation_thread(ProtectedData* pdata)
{
	while (true)
	{
		static double count = 0;
		unique_lock<mutex> lk(pdata->mut);
		pdata->data_cond.wait(lk, [pdata]{return pdata->data_que.size() <= MAXNUM; });
		pdata->data_que.push(count++);
		cout << "Adding elements: "  << count << endl;
		pdata->data_cond.notify_one();
		lk.unlock();
	}
}

void data_processing_thread(ProtectedData* pdata)
{
	while (true)
	{
		unique_lock<mutex> lk(pdata->mut);
		pdata->data_cond.wait(lk, [pdata]{return !(pdata->data_que).empty(); });
		double popeddata = pdata->data_que.front();
		pdata->data_que.pop();
		cout << "Popped data: " << popeddata << endl;
		cout << "Current size: " << pdata->data_que.size() << endl;
		lk.unlock();
	}
}

int main()
{
	ProtectedData data_chunk;
	data_chunk.init();
	thread t1(data_processing_thread, &data_chunk);
	thread t2(data_preparation_thread, &data_chunk);

}
