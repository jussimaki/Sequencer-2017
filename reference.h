#ifndef REFERENCE_H
#define REFERENCE_H

#include "entry_holder.h"
#include "id_issuer.h"

template <typename dataType>
class Reference
{
public:
	Reference(dataType *object, EntryHolder <dataType> *entryHolder) : entryHolder_(entryHolder)
	{
		std::cout << "reference constructor" << std::endl;
		ID_ = entryHolder_->getIDIssuer()->issue();
		entryHolder_->setObject(ID_, object);
	}

	~Reference()
	{
		entryHolder_->returnID(ID_);
	}

	// assignment operator
	// this is called when 'a = b;' after the construction of a
	// not when 'Reference a = b;'
	Reference& operator=(const Reference& other)
	{

		std::cout << "reference assignment" << std::endl;

		if (this != &other)
		{
			// the old reference is lost so the ID must
			// be returned and the refCount must be decreased

			if (entryHolder_)
			{
				entryHolder_->returnID(ID_);
			}
			
			entryHolder_ = other.entryHolder_;
			ID_ = other.ID_;

			entryHolder_->increaseRefCount(ID_);
		}

		return *this;
	}
	
	// copy constructor
	// this is called when either 'Reference a = b;'
	// or 'Reference a(b);'
	Reference(const Reference &other)
	{

		std::cout << "copy constructor" << std::endl;


		if (this != &other)
		{
			// NO WAY!
			// this should be in reassignment operator
			// since 'entryHolder_' pointer address is probably
			// 'garbage' (and not 'zero') at this point

			/*
			if (entryHolder_)
			{
				entryHolder_->returnID(ID_);
			}
			*/

			entryHolder_ = other.entryHolder_;
			ID_ = other.ID_;

			entryHolder_->increaseRefCount(ID_);
		}
	}

	unsigned int getID() { return ID_; }
	dataType *acquireObject()
	{
		return entryHolder_->acquireObject(ID_);
	}

	void deleteObject()
	{
		entryHolder_->deleteObject(ID_);
	}

	unsigned int getRefCount()
	{
		return entryHolder_->getRefCount(ID_);
	}

private:
	EntryHolder <dataType> *entryHolder_;

	unsigned int ID_;
};


#endif
