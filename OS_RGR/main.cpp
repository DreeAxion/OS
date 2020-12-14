#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <ctime>


#define BUFF_SIZE 1

void Producer(std::stack<int>& buffer, std::mutex& mutex_lock, std::atomic<int>& SemaphoreFull, std::atomic<int>& SemaphoreEmpty);

void Consumer(std::stack<int>& buffer, std::mutex& mutex_lock, std::atomic<int>& SemaphoreFull, std::atomic<int>& SemaphoreEmpty);


int main()
{
	system("chcp 1251>null");


	std::stack<int> buffer;

	std::mutex mutex_lock;
	std::atomic<int> SemaphoreEmty = BUFF_SIZE;
	std::atomic<int> SemaphoreFull = 0;




	std::thread prod(Producer, std::ref(buffer), std::ref(mutex_lock), std::ref(SemaphoreFull), std::ref(SemaphoreEmty));
	std::thread cons(Consumer, std::ref(buffer), std::ref(mutex_lock), std::ref(SemaphoreFull), std::ref(SemaphoreEmty));


	prod.join();
	cons.join();
	


	return 1;
}


void Producer(std::stack<int>& buffer, std::mutex& mutex_lock, std::atomic<int>& SemaphoreFull, std::atomic<int>& SemaphoreEmpty)
{
	int item = 999999;
	mutex_lock.lock();

	std::cout << "\n Производитель создал: " << item;
	std::cout << " (Буфер: " << buffer.size() << " из " << BUFF_SIZE << ")\n";

	bool flagPush{ false };
	mutex_lock.unlock();

	while (!flagPush)
	{


		if (SemaphoreEmpty)
		{
			mutex_lock.lock();

			buffer.push(item);
			SemaphoreEmpty.fetch_sub(1);
			SemaphoreFull.fetch_add(1);

			std::cout << "\n Производитель поместил " << item << " в буфер\n";
			flagPush = true;

			mutex_lock.unlock();
		}

	}

};


void Consumer(std::stack<int>& buffer, std::mutex& mutex_lock, std::atomic<int>& SemaphoreFull, std::atomic<int>& SemaphoreEmpty)
{
	mutex_lock.lock();
	std::cout << "\n Потребитель запросил данные из буфера: ";
	std::cout << " (Буфер: " << buffer.size() << " из " << BUFF_SIZE << ")\n";
	mutex_lock.unlock();


	bool flagPop{ false };
	int item;

	while (!flagPop)
	{


		if (SemaphoreFull)
		{
			mutex_lock.lock();

			item = buffer.top();
			buffer.pop();
			SemaphoreFull.fetch_sub(1);

			std::cout << "\n Потребитель забрал " << item << " из буфера\n";
			flagPop = true;
			SemaphoreEmpty.fetch_add(1);
			mutex_lock.unlock();
		}

	}

};



