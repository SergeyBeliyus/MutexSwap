#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>

class Data {
public:
	Data(std::vector<int> data) : data{ data } {}
private:
	friend void swap1(Data& lhs, Data& rhs);
	friend void swap2(Data& lhs, Data& rhs);
	friend void swap3(Data& lhs, Data& rhs);

	std::vector<int> data;
	std::mutex m;
};

void swap1(Data& lhs, Data& rhs);

void swap2(Data& lhs, Data& rhs);

void swap3(Data& lhs, Data& rhs);

int main()
{
	std::vector<int> data1{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::vector<int> data2{ 11, 12, 13, 14, 15, 16, 17, 18, 19 };
	Data d1(data1), d2(data2);
	std::thread t1(swap1, std::ref(d1), std::ref(d2));
	std::thread t2(swap2, std::ref(d1), std::ref(d2));
	std::thread t3(swap3, std::ref(d1), std::ref(d2));
	t1.join();
	t2.join();
	t3.join();
	return 0;
}

void swap1(Data& lhs, Data& rhs)
{
	lhs.m.lock();
	rhs.m.lock();
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
	lhs.m.unlock();
	rhs.m.unlock();
}

void swap2(Data& lhs, Data& rhs)
{
	std::scoped_lock lock(lhs.m, rhs.m);
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
}

void swap3(Data& lhs, Data& rhs)
{
	std::unique_lock ul1(lhs.m);
	std::unique_lock ul2(rhs.m);
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
	ul1.unlock();
	ul2.unlock();
}