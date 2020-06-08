#ifndef SPAWN_THREAD_H
#define SPAWN_THREAD_H

#include <atomic>
#include <thread>

template <typename... Args>
struct Impl;

template <typename First, typename... Rest>
struct Impl<First, Rest...>
{
	First firstArg_;
	Impl <Rest...> restOfArgs_;

	Impl(First firstArg, Rest... restOfArgs) : firstArg_(firstArg), restOfArgs_(restOfArgs...) {}

	First getFirst()
	{
		return firstArg_;
	}
	
	template <typename F, typename... Total> void call(F function, Total ...list)
	{
		//Impl<Rest...>::call(function, list..., restOfArgs_.getFirst());
		restOfArgs_.call(function, list..., getFirst());
	}
};

template <typename First>
struct Impl<First>
{
	First firstArg_;

	Impl(First firstArg) : firstArg_(firstArg) {}

	First getFirst()
	{
		return firstArg_;
	}

	template <typename F, typename... Total> void call(F function, Total ...list)
	{
		function(list..., getFirst());
	}
};

template <>
struct Impl<>
{
	template <typename F, typename... Total> void call(F function, Total ...list)
	{
	}
};


extern std::atomic <int> threadCount;

template <typename T, typename... Args>
class ThreadClass
{
private:
	
	Impl<Args...> arguments_;
	T function_;

public:
	ThreadClass(T function, Args... args) : arguments_(args...), function_(function)
	{
	}

	~ThreadClass()
	{
		int oldValue = 0;
		int newValue = 0;

		do
		{
			oldValue = threadCount.load();
			newValue = oldValue - 1;

		} while (!threadCount.compare_exchange_weak(oldValue, newValue, std::memory_order_acquire));
	}


	void execute()
	{
		arguments_.call(function_);
	}
	
};

class COUNTED_THREAD
{
public:

	template <typename T, typename... Args>
	void static EXTRAFUNCTION(T function, Args... args)
	{
		try
		{
			ThreadClass<T, Args...> threadObject(function, args...);
			threadObject.execute();
		}
		catch (std::exception& e) {
			//*exception = std::current_exception();
		}
	}

	COUNTED_THREAD(){}

	template <typename T, typename... Args>
	COUNTED_THREAD(T function, Args... args)
	{

		int oldValue = 0;
		int newValue = 0;

		do
		{
			oldValue = threadCount.load();

			if (oldValue == -1) return;

			newValue = oldValue + 1;

		} while (!threadCount.compare_exchange_weak(oldValue, newValue, std::memory_order_acquire));

		std::thread thread(&COUNTED_THREAD::EXTRAFUNCTION<T, Args...>, function, args...);
		thread.detach();
	}

};


#endif
