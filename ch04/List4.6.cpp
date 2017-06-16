// Copyright
// Author : Aperion_wang
// Email : wangwei_aperion@163.com
#include <future>
#include <iostream>
using namespace std;
int find_the_answer(int i)
{
	int a = 0;
	for (int j = 0; j < i; j++)
	{
		a++;
	}

	return a;
}

void do_other()
{
	for (int i = 0; i < 100; i++)
	{
		cout << "In do_other(): " << i << endl;
	}
}

int main()
{
	future<int> variable = async(find_the_answer, 100000);
	do_other();
	cout << "The answer is " << variable.get() << std::endl;
}
