#include "cas_lock.h"


bool CASLock::tryToLock()
{
	unsigned int unlocked = 0;

	return lock_.compare_exchange_weak(unlocked, 1, std::memory_order_acquire);
}

void CASLock::lock()
{
	unsigned int unlocked = 0;

	while (!lock_.compare_exchange_weak(unlocked, 1, std::memory_order_acquire))
	//while (!lock_.compare_exchange_weak(unlocked, 1))
	{
		unlocked = 0;
	}
}

void CASLock::unlock()
{
	lock_ = 0;
}