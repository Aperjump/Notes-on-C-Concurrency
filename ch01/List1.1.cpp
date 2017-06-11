/***
//
// @author Aperion Wang
// @date   2017-06-10
***/
#include <iostream>
#include <thread>

using namespace std;
void hello(int i)
{
  cout << "In thread No." << i << endl;
}

int main()
{
  thread t1(hello, 10);
  thread t2(hello, 12);
  thread t3(hello, 14);
  hello(15);
  t1.join();
  t2.join();
  t3.join();
}
