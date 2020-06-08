#ifndef LOCK_GUARD_H
#define LOCK_GUARD_H


class LockHierarchy;
class CASLock;

class LockGuard
{
public:
	LockGuard(LockHierarchy *hierarchy, CASLock **locks, unsigned int amount, unsigned int level);
	~LockGuard();
	bool error();
private:
	LockGuard();

	LockHierarchy *hierarchy_;
	CASLock **locks_;
	bool error_;
};

#endif
