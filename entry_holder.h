#ifndef ENTRY_HOLDER_H
#define ENTRY_HOLDER_H

#include "id_issuer.h"

template <typename dataType>
class Entry
{
public:
	Entry() : refCount_(0), object_(0)
	{}

	void deleteObject()
	{
		if (object_)
		{
			delete object_;
			object_ = 0;
		}
	}

	dataType *acquireObject()
	{
		return object_;
	}

	void setObject(dataType *object)
	{
		object_ = object;
	}

	void decreaseRefCount()
	{
		refCount_--;
	}

	void increaseRefCount()
	{
		refCount_++;
	}

	unsigned int getRefCount()
	{
		return refCount_;
	}

	void zeroRefCount()
	{
		refCount_ = 0;
	}

private:
	unsigned int refCount_;
	dataType *object_;
};

template <typename dataType>
class EntryHolder
{
public:
	EntryHolder(IDIssuer <dataType> *issuer) : issuer_(issuer)
	{
		entryAmount_ = issuer_->getIDAmount();

		entries_ = new Entry<dataType>[entryAmount_];
	}

	EntryHolder()
	{
		delete entries_;
		entries_ = 0;
	}

	IDIssuer <dataType> *getIDIssuer() { return issuer_; }

	dataType *acquireObject(unsigned int ID)
	{
		return entries_[ID].acquireObject();
	}

	void setObject(unsigned int ID, dataType *object)
	{
		entries_[ID].setObject(object);
		entries_[ID].zeroRefCount();
		entries_[ID].increaseRefCount();
	}

	void deleteObject(unsigned int ID)
	{
		entries_[ID].deleteObject();
	}

	unsigned int getRefCount(unsigned int ID)
	{
		return entries_[ID].getRefCount();
	}
	void increaseRefCount(unsigned int ID)
	{
		entries_[ID].increaseRefCount();
	}

	// this is called when a reference is destroyed
	void returnID(unsigned int ID)
	{
		entries_[ID].decreaseRefCount();

		// if all the references carrying the ID are destroyed
		// then actually return the ID to the issuer
		if (entries_[ID].getRefCount() == 0)
		{
			if (entries_[ID].acquireObject())
			{
				entries_[ID].deleteObject();
			}

			issuer_->returnID(ID);
		}
	}
private:
	IDIssuer <dataType> *issuer_;
	unsigned int entryAmount_;
	Entry <dataType> *entries_;
};

#endif
