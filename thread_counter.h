#ifndef THREAD_COUNTER_H
#define THREAD_COUNTER_H


class ThreadCounter
{
public:
	ThreadCounter(std::atomic <int> *pointer) : pointer_(pointer)
	{
		std::cout << "thread++";
		
		int oldValue, newValue;
		do
		{
			oldValue = (*pointer_).load();
			newValue = oldValue + 1;

		} while(!(*pointer_).compare_exchange_weak(oldValue, newValue, std::memory_order_acquire));
	}

	~ThreadCounter()
	{
		std::cout << "thread--";

		int oldValue, newValue;
		do
		{
			oldValue = (*pointer_).load();
			newValue = oldValue - 1;

		} while (!(*pointer_).compare_exchange_weak(oldValue, newValue, std::memory_order_acquire));
	}

	int getCount()
	{
		return (*pointer_).load();
	}

private:
	std::atomic <int> *pointer_;


};


#endif

