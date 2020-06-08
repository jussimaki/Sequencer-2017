#ifndef LOCK_HIERARCHY_H
#define LOCK_HIERARCHY_H

#include <vector>

#include "cas_lock.h"

struct LockNode
{
	CASLock *lock;
	bool acquired;
};

struct LockLevel
{
	unsigned int lastLevel;
	std::vector <LockNode> nodes;
};

class LockHierarchy
{
public:

	LockHierarchy(unsigned int levelAmount);
	LockHierarchy(LockHierarchy *original);
	~LockHierarchy();

	bool addLock(CASLock *lock, unsigned int level);


	bool tryToAcquire(CASLock *locks[], unsigned int amount, unsigned int level);

	bool acquire(CASLock *locks[], unsigned int amount, unsigned int level);
	bool isLockAcquired(CASLock *lock, unsigned int level);

	void release();
	void setReady();
	//bool Acquire(CASLock *lock, unsigned int level);

	unsigned int getCurrentLevel();
	bool isLevelAcquired(unsigned int level);

	void printLocks();
	LockHierarchy *getOriginal() { return original_; }
private:
	LockHierarchy();

	unsigned int currentLevel_;
	bool ready_;
	LockHierarchy *original_;
	unsigned int levelAmount_;
	std::vector <LockLevel> levels_;

	bool get();
};

#endif