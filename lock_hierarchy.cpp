#include <iostream>

#include <assert.h>

#include "lock_hierarchy.h"

//extern std::mutex coutMutex;
LockHierarchy::LockHierarchy(unsigned int levelAmount) : currentLevel_(levelAmount), ready_(false), original_(0), levelAmount_(levelAmount)
{
	for (unsigned int index = 0; index < levelAmount_; index++)
	{
		LockLevel level;

		level.lastLevel = levelAmount;

		levels_.push_back(level);
	}

}

LockHierarchy::LockHierarchy(LockHierarchy *original) : currentLevel_(0), ready_(false), original_(original), levelAmount_(0)
{
	/*
	coutMutex.lock();
	std::cout << "thread specific hierarchy" << std::endl;

	std::cout << "ID: " << std::this_thread::get_id() << std::endl;

	if(!get()) std::cout << "couldn't copy hierarchy yet" << std::endl;
	else std::cout << "hierarchy copied" << std::endl;
	

	coutMutex.unlock();
	*/

	get();
}

LockHierarchy::~LockHierarchy()
{
	/*
	std::cout << "~LockHierarchy" << std::endl;
	std::cout << "ID: " << std::this_thread::get_id() << std::endl;

	*/
}



bool LockHierarchy::addLock(CASLock *lock, unsigned int level)
{
	if (levelAmount_ <= level) return false;

	LockNode node;
	node.acquired = false;
	node.lock = lock;

	levels_[level].nodes.push_back(node);

	return true;
}


bool LockHierarchy::tryToAcquire(CASLock *locks[], unsigned int amount, unsigned int level)
{
	if (!ready_)
	{
		//std::cout << "get lock later" << std::endl;
		//assert((get()));
		if (!get()) return false;
		//std::cout << "success" << std::endl;
	}

	assert(level < currentLevel_);

	if (level >= currentLevel_)
	{
		//std::cout << "cant change level. current level is " << level << std::endl;
		return false;
	}

	

	for (unsigned int index = 0; index < levels_[level].nodes.size(); index++)
	{
		CASLock *lock = levels_[level].nodes[index].lock;

		for (unsigned int acquireIndex = 0; acquireIndex < amount; acquireIndex++)
		{
			if (lock == locks[acquireIndex])
			{
				
				if (lock->tryToLock())
				{
					levels_[level].nodes[index].acquired = true;
				}
				else
				{
					// couldn't acquire the lock. unlock all the acquired locks
					// in reverse order.

					for (unsigned int reverseIndex = index; reverseIndex >= 0; reverseIndex--)
					{
						if (levels_[level].nodes[reverseIndex].acquired)
						{
							levels_[level].nodes[reverseIndex].lock->unlock();
							levels_[level].nodes[reverseIndex].acquired = false;
						}

						if (reverseIndex == 0) break;
					}

					return false;
				}
			}
		}
	}
		
	levels_[level].lastLevel = currentLevel_;
	currentLevel_ = level;
	return true;

}


bool LockHierarchy::acquire(CASLock *locks[], unsigned int amount, unsigned int level)
{
	if (!ready_)
	{
		//std::cout << "get lock later" << std::endl;
		//assert((get()));
		if (!get()) return false;
		//std::cout << "success" << std::endl;
	}

	assert(level < currentLevel_);

	if (level >= currentLevel_)
	{
		//std::cout << "cant change level. current level is " << level << std::endl;
		return false;
	}

	//std::cout << "level " << level << std::endl;
	
	assert(levels_[level].nodes.size() > 0);
	unsigned int lockedAmount = 0;
	for (unsigned int index = 0; index < levels_[level].nodes.size(); index++)
	{
		CASLock *lock = levels_[level].nodes[index].lock;
		
		//std::cout << "lock " << lock << " - ";

		for (unsigned int acquireIndex = 0; acquireIndex < amount; acquireIndex++)
		{
			if (lock == locks[acquireIndex])
			{
				lockedAmount++;
				lock->lock();
				//std::cout << " locked. ";
				levels_[level].nodes[index].acquired = true;
			}
		}
		
		//std::cout << std::endl;
	}

	assert(lockedAmount > 0);
	assert(lockedAmount == amount);
	levels_[level].lastLevel = currentLevel_;
	currentLevel_ = level;
	return true;

}

bool LockHierarchy::isLockAcquired(CASLock *lock, unsigned int level)
{
	for (unsigned int index = 0; index < levels_[level].nodes.size(); index++)
	{
		if (levels_[level].nodes[index].lock == lock)
		{
			if(levels_[level].nodes[index].acquired) return true;
			else return false;
		}
	}
	return false;
}

void LockHierarchy::release()
{
	if (!ready_)
	{
		if (!get()) return;
	}

	//std::cout << "release level " << currentLevel_ << std::endl;

	if (currentLevel_ < levelAmount_)
	{
	//	std::cout << levels_[currentLevel_].nodes.size() << " nodes." << std::endl;

		if (levels_[currentLevel_].nodes.size() > 0)
		{
			unsigned int index = levels_[currentLevel_].nodes.size() - 1;
			while (true)
			{
				//	std::cout << "index: " << index << std::endl;
				if (levels_[currentLevel_].nodes[index].acquired)
				{
					levels_[currentLevel_].nodes[index].lock->unlock();
					levels_[currentLevel_].nodes[index].acquired = false;
				}

				if (index == 0) break;
				index--;
			}

		}
		currentLevel_ = levels_[currentLevel_].lastLevel;

		//std::cout << "currentLevel_ -> " << currentLevel_ << std::endl;
	}
}

void LockHierarchy::setReady()
{
	ready_ = true;
}

unsigned int LockHierarchy::getCurrentLevel()
{
	return currentLevel_;
}

bool LockHierarchy::isLevelAcquired(unsigned int level)
{
	
	unsigned int tempLevel = currentLevel_;
	while (tempLevel != levelAmount_)
	{
		if (tempLevel == level) return true;
		else tempLevel = levels_[tempLevel].lastLevel;
	}

	return false;
}


void LockHierarchy::printLocks()
{
	if (!ready_)
	{
		if (!get()) return;
	}

	for (unsigned int level = 0; level < levels_.size(); level++)
	{
		std::cout << "level " << level << ":" << std::endl;

		for (unsigned int index = 0; index < levels_[level].nodes.size(); index++)
		{
			std::cout << levels_[level].nodes[index].lock << " ";
		}

		std::cout << std::endl;
	}
}


bool LockHierarchy::get()
{
	if (original_->ready_)
	{
		levelAmount_ = original_->levelAmount_;
		currentLevel_ = original_->currentLevel_;
		levels_ = original_->levels_;

		ready_ = true;
	}
	else ready_ = false;

	return ready_;
}
/*
bool LockHierarchy::Acquire(CASLock *lock, unsigned int level)
{
	if (levelAmount_ <= level) return false;

	if (!levels_[level].acquired)
	{
		for (unsigned int levelIndex = levelAmount_ - 1; levelIndex > level; levelIndex--)
		{
			if (levels_[levelIndex].acquired) return false;
		}
	}

	levels_[level].acquired = true;

	for (unsigned int index = 0; index <= levels_[level].nodes.size(); index++)
	{
		if (!levels_[level].nodes[index].acquired)
		{
			levels_[level].nodes[index].lock->lock();
			levels_[level].nodes[index].acquired = true;
		}

		if (levels_[level].nodes[index].lock == lock) break;
	}

	return true;
}*/