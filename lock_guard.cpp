#include <assert.h>

#include "lock_guard.h"
#include "lock_hierarchy.h"

LockGuard::LockGuard(LockHierarchy *hierarchy, CASLock **locks, unsigned int amount, unsigned int level) : hierarchy_(hierarchy), locks_(locks), error_(false)
{
	error_ = !(hierarchy_->acquire(locks_, amount, level));

	//assert(!(error_ = !(hierarchy_->acquire(locks_, amount, level))));

	assert(!error_);
}

bool LockGuard::error()
{
	return error_;
}
LockGuard::~LockGuard()
{
	hierarchy_->release();
}