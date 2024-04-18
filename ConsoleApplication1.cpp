#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <Windows.h>
#include <random>
#include <map>

using namespace std::chrono_literals;

void SetColor(int text, int bg) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 1) | text));
}

class Data {
public:
	Data(std::vector<int> data) : data{ data } {}
private:
	friend void swap1(Data& lhs, Data& rhs);
	friend void swap2(Data& lhs, Data& rhs);
	friend void swap3(Data& lhs, Data& rhs);
	friend void progressBar(Data& lhs, Data& rhs);

	std::vector<int> data;
	std::mutex m;
};

void swap1(Data& lhs, Data& rhs);

void swap2(Data& lhs, Data& rhs);

void swap3(Data& lhs, Data& rhs);

void progressBar(Data& lhs, Data& rhs);

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
void progressBar(Data& lhs, Data& rhs) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 100);
	char fillingChar = '|';
	char leftBoundary = '[';
	char rightBoundary = ']';
	int width = 50; 
	int current = 0; 
	int total = 100; 
	int step = total / (lhs.data.size() - 1); 
	int progress = 0;
	std::vector<std::pair<std::string, int>> bar;
	int previousFill = 0;
	for (int i = 0; i < lhs.data.size(); ++i) {
		double percent = (progress * 100) / total;
		percent = (i == lhs.data.size() - 1 && percent != 100) ? 100 : (progress * 100) / total;
		int fill = (percent * width) / 100;
		try {
			if (dis(gen) % 3 == 0) {
				throw std::runtime_error("Error");
			}
			int tmp = lhs.data.at(i);
			lhs.data.at(i) = rhs.data.at(i);
			rhs.data.at(i) = tmp;
			bar.push_back(std::pair(std::string(fill - previousFill, fillingChar), 2));
		}
		catch (const std::exception& e) {
			bar.push_back(std::pair(std::string(fill - previousFill, fillingChar), 4));
		}
		SetColor(3, 3);
		std::cout << "\r" << leftBoundary;
		for (int j = 0; j < bar.size(); ++j) {
			SetColor(bar.at(j).second, 0);
			for (int c = 0; c < bar.at(j).first.size(); ++c) {
				if (j != bar.size() - 1) {
					std::this_thread::sleep_for(0ms);
				}
				else {
					std::this_thread::sleep_for(100ms);
				}
				std::cout << bar.at(j).first.at(c);
			}
		}
		for (int j = fill; j < width; j++) {
			SetColor(3, 3);
			std::cout << " ";
		}
		SetColor(3, 3);
		std::cout << rightBoundary << " " << percent << "%";
		current += step;
		progress += step;
		previousFill = fill;

	}
	std::cout << std::endl;
	SetColor(3, 3);
}
void swap1(Data& lhs, Data& rhs)
{
	std::lock(lhs.m, rhs.m);
	std::lock_guard<std::mutex> lk1(lhs.m, std::adopt_lock);
	std::lock_guard<std::mutex> lk2(rhs.m, std::adopt_lock);
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
	progressBar(lhs, rhs);
}

void swap2(Data& lhs, Data& rhs)
{
	std::scoped_lock sclock(lhs.m, rhs.m);
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
	progressBar(lhs, rhs);
}

void swap3(Data& lhs, Data& rhs)
{
	std::unique_lock ul1(lhs.m);
	std::unique_lock ul2(rhs.m);
	std::lock(ul1, ul2);
	std::cout << "Thread's id: " << std::this_thread::get_id() << '\n';
	progressBar(lhs, rhs);
}