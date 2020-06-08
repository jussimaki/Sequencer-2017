#include <iostream>

#include "test2.h"

#include "lock_guard.h"
#include "lock_hierarchy.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;

void test2(unsigned int number, CASLock *coutLockP, CASLock *eventLockP)
{
	while (1)
	{
		LockGuard eventGuard(&localLockHierarchy, &eventLockP, 1, LOCK_LEVELS::EVENTS);
		LockGuard coutGuard(&localLockHierarchy, &coutLockP, 1, LOCK_LEVELS::IO);
		
		

		std::cout << "worker " << number << " got coutlock" << std::endl;
		std::cout << "worker " << number << " local lock hierarchy: " << &localLockHierarchy << std::endl;
		std::cout << "worker " << number << " original lock hierarchy: " << localLockHierarchy.getOriginal() << std::endl;
		//localLockHierarchy.printLocks();
	}
}

