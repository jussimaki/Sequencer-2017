#ifndef SWITCHABLE_H
#define SWITCHABLE_H

#include <atomic>

template <typename dataType>
class Switchable
{
public:
	Switchable() : working_(false), current_(new dataType), next_(0), trash_(0){}

	Switchable(dataType *data)
	{
		working_ = false;
		current_ = data;
		next_ = 0;
		trash_ = 0;
	}

	~Switchable()
	{
		if (current_)
		{
			delete current_;
			current_ = 0;
		}

		if (next_)
		{
			delete next_;
			next_ = 0;
		}

		if (trash_)
		{
			delete trash_;
			trash_ = 0;
		}
	}

	// Called in worker thread
	void setNext(dataType *newData)
	{
		bool expected = false;

		// must acquire the lock to change the value
		while (!working_.compare_exchange_weak(expected, true))
		{
			expected = false;
		}


		if (trash_)
		{
			delete trash_;
			trash_ = 0;
		}

		if (next_) delete next_;

		next_ = newData;


		working_ = false;
	}


	// GUI thread calls this
	dataType *getCurrentData()
	{
		return current_;
	}


	// GUI thread calls this
	void switchData()
	{
		bool expected = false;
		
		// not necessarily acquired. no time to wait to acquire the lock in GUI thread.
		bool acquired = working_.compare_exchange_weak(expected, true);

		if (acquired)
		{
			if (next_)
			{
				trash_ = current_; // no time to delete stuff in GUI thread. move it to trash.
				current_ = next_;
				next_ = 0;
			}


			working_ = false;
		}


	}

private:
	std::atomic <bool> working_;


	dataType *current_;
	dataType *next_;
	dataType *trash_;
};



#endif
