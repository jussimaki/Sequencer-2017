#ifndef ID_H
#define ID_H

#include <assert.h>

#include "red_black_tree.h"
#include "cas_lock.h"
#include "lock_hierarchy.h"
#include "lock_guard.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;


template <class dataType>
class IDIssuer
{
public:
	IDIssuer(CASLock *identityLock, long amount) : identityLock_(identityLock)
	{
		for (long index = 0; index < amount; index++)
		{
			RedBlackNode <long> *node = new RedBlackNode<long>;

			node->data = 0; // this is the number of references to the ID
			node->value = index;
						
			availableIDs_.insert(node);
		}
		
	}

	long issueID(bool *success)
	{
		*success = false;

		RedBlackNode <long> *node = availableIDs_.getRoot();
		
		if (node) // root found
		{

			RedBlackNode <long> *removedNode = availableIDs_.remove(node);

			if (removedNode) // a node is removed from the availableIDs tree
			{

				*success = true;

				removedNode->data = 1; // first reference to this ID

				issuedIDs_.insert(removedNode);

				return removedNode->value;
			}
		}
		
		return 0;
	}

	void returnID(long value)
	{
		RedBlackNode <long> *node = issuedIDs_.find(value);

		if (node)
		{
			node->data--; // decrease the reference count

			if (node->data == 0) // if there are no more references to the ID, return the node
			{

				RedBlackNode <long> *removedNode = issuedIDs_.remove(node);

				if (removedNode)
				{
					availableIDs_.insert(removedNode);
				}
			}
		}
	}

	void increaseCount(long value)
	{
		RedBlackNode <long> *node = issuedIDs_.find(value);

		if (node)
		{
			node->data++; // increase the reference count
		}
	}

	CASLock *getIdentityLock() { return identityLock_; }
private: 
	CASLock *identityLock_;

	RedBlackTree <long> availableIDs_;
	RedBlackTree <long> issuedIDs_;

};

template <class dataType>
class ID
{
public:
	ID(IDIssuer <dataType>*issuer) : //issuer_(issuer), identityLock_(issuer_->getIdentityLock()), empty_(true), error_(false), value_(0)
		issuer_(issuer), empty_(true), error_(false), value_(0)
	{

		if (issuer_)
		{
			identityLock_ = issuer_->getIdentityLock();
		}
		else
		{
			identityLock_ = 0;
			error_ = true;
		}

	}
	/*
	ID() : issuer_(0), identityLock_(0), empty_(true), error_(false), value_(0)
	{
	}
	*/
	void acquireID()
	{
		if (empty_ && (!error_))
		{
			bool success;

			LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);

			value_ = issuer_->issueID(&success);

			if (success) empty_ = false;
			error_ = !success;
		}
	}

	void returnID()
	{
		if(!empty_)
		{
			LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);
			issuer_->returnID(value_);

			value_ = 0;
			empty_ = true;
			error_ = false;
		}
	}

	bool isEmpty()
	{
		LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);

		return empty_;
	}

	~ID()
	{
		if (error_) return; // no identityLock_ or issuer_
		LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);

		if (!empty_) issuer_->returnID(value_);
	}
	
	
	// equals operator
	bool operator==(const ID<dataType>& other)
	{
		LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);

		if (error_ || other.error_) return false;
		if (empty_ || other.empty_) return false;

		if (value_ == other.value_) return true;
		else return false;
	}

	// assignment operator
	ID<dataType>& operator=(const ID<dataType>& other)
	{
		if (error_ && !other.error_)
		{
			issuer_ = other.issuer_;
			identityLock_ = issuer_->getIdentityLock();

			error_ = false;
		}


		if (error_) return *this;

		
		LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);
		
		if (!empty_)
		{
			issuer_->returnID(value_);
		}

		empty_ = other.empty_;
		error_ = other.error_;
		value_ = other.value_;

		

		if (!empty_)
		{
			issuer_->increaseCount(value_);
		}

		return *this;
	}

	// copy constructor
	ID(const ID <dataType> &other)
	{
		issuer_ = other.issuer_;
		identityLock_ = other.identityLock_;

		empty_ = other.empty_;
		error_ = other.error_;
		value_ = other.value_;
		
		if (!empty_)
		{
			LockGuard guard(&localLockHierarchy, &identityLock_, 1, LOCK_LEVELS::IDENTITY);

			issuer_->increaseCount(value_);
		}
	}

	bool isValid() { return !error_; }

	long getValue()
	{
		//CASLock *lock = identityLock_;

		//LockGuard guard(&localLockHierarchy, &lock, 1, LOCK_LEVELS::IDENTITY);
		return value_;
	}

	CASLock *getIdentityLock() {
		return identityLock_;
	}

	

private:

	IDIssuer <dataType>*issuer_;
	void *pointer;
	CASLock *identityLock_;

	bool empty_;
	bool error_;
	long value_;

	ID();

	
};

#endif
