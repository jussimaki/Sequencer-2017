#ifndef CAS_LOCK_H
#define CAS_LOCK_H

#include <atomic>

class CASLock
{
public:
	CASLock() : lock_(0) {}
	bool tryToLock();
	void lock();
	void unlock();
private:
	std::atomic <unsigned int> lock_;
};

#endif
