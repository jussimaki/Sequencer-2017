#ifndef ID_ISSUER_H
#define ID_ISSUER_H

#include "pool.h"

#include "cas_lock.h"

template <typename dataType>
class IDIssuer
{
public:
	IDIssuer(unsigned int IDAmount, CASLock *IDLock) : IDAmount_(IDAmount), IDLock_(IDLock)
	{

		for (unsigned int index = 0; index < IDAmount_; index++)
		{
			Node <unsigned int> *IDNode = new Node <unsigned int>;

			IDNode->data = index;

			availableIDs_.addToEnd(IDNode);
		}
	}

	~IDIssuer()
	{
		zeroPool_.clear();
		availableIDs_.clear();
	}

	unsigned int getIDAmount() { return IDAmount_; }

	unsigned int issue()
	{

		Node <unsigned int> *node = availableIDs_.extractFirstNode();
		unsigned int value = node->data;
		
		node->data = 0;

		zeroPool_.addToEnd(node);

		return value;
	}

	void returnID(unsigned int ID)
	{

		Node <unsigned int> *node = zeroPool_.extractFirstNode();
		node->data = ID;
		availableIDs_.addToBegin(node);
	}

private:
	unsigned int IDAmount_;
	CASLock *IDLock_;

	Pool <unsigned int> zeroPool_;
	Pool <unsigned int> availableIDs_;
};

#endif
