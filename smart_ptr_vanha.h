#ifndef SMART_PTR_H
#define SMART_PTR_H

#include <iostream>

class CBlock
{
public:
	CBlock(void *object) : refCount_(1), object_(object) {}
	CBlock() : refCount_(0), object_(0) {}

	void set(void *object)
	{
		refCount_ = 1;
		object_ = object;
	}

	void *getObject() { return object_; }
	void setObjectToZero() { object_ = 0; }

	void increaseRefCount() { refCount_++; }
	void decreaseRefCount() { refCount_--; }
	
	unsigned int getRefCount() { return refCount_; }

private:
	
	unsigned int refCount_;
	void *object_;
	
};

template <typename dataType>
class EnableSmartFromThis;

template <typename dataType>
class SmartPtr
{
	template <class Base>
	friend class SmartPtr;

public:
	SmartPtr()
	{
		cBlock_ = new CBlock;
	}

	SmartPtr(dataType *object)
	{
		cBlock_ = new CBlock(object);

		EnableSmart(this);
	}

	void setSmart(dataType *object)
	{
		cBlock_->set(object);
	}

	

	void EnableSmart(SmartPtr<EnableSmartFromThis<dataType>> *pointer)
	{
		std::cout << "template function" << std::endl;
	}

	void EnableSmart(void *pointer)
	{
		// do nothing
	}


	~SmartPtr()
	{
		cBlock_->decreaseRefCount();

		if (cBlock_->getRefCount() == 0)
		{
			if (cBlock_->getObject())
			{
				delete ((dataType*)cBlock_->getObject());
			}

			delete cBlock_;

		}
	}

	void deleteObject()
	{
		if (cBlock_->getObject())
		{
			std::cout << "delete object" << std::endl;
			delete ((dataType*)cBlock_->getObject());
			cBlock_->setObjectToZero();
		}
	}
	
	// assignment operator
	SmartPtr& operator=(const SmartPtr& other)
	{
		std::cout << "assignment operator" << std::endl;
		if (this != &other)
		{
			cBlock_->decreaseRefCount();

			if (cBlock_->getRefCount() == 0)
			{
				if (cBlock_->getObject())
				{
					delete ((dataType*)cBlock_->getObject());
				}
				delete cBlock_;
			}
			
			cBlock_ = other.cBlock_;

			cBlock_->increaseRefCount();

		}
		else std::cout << "same object!" << std::endl;

		return *this;
	}

	// copy constructor
	SmartPtr(const SmartPtr &other)
	{
		std::cout << "copy constructor" << std::endl;

		cBlock_ = other.cBlock_;

		cBlock_->increaseRefCount();
	}
	


	// assignment operator
	template <typename Derived>
	SmartPtr& operator=(const SmartPtr<Derived>& other)
	{
		std::cout << "assignment operator derived" << std::endl;
		if ((void*)this != (void*)&other)
		{
			cBlock_->decreaseRefCount();

			if (cBlock_->getRefCount() == 0)
			{
				if (cBlock_->getObject())
				{
					delete ((dataType*)cBlock_->getObject());
				}
				delete cBlock_;
			}

			cBlock_ = other.cBlock_;

			cBlock_->increaseRefCount();

		}
		else std::cout << "same object!" << std::endl;

		dataType *pointer = (Derived*)cBlock_->getObject();

		return *this;
	}


	// copy constructor
	template <typename Derived>
	SmartPtr(const SmartPtr<Derived> &other)
	{
		std::cout << "copy constructor derived" << std::endl;
		cBlock_ = other.cBlock_;

		cBlock_->increaseRefCount();
	
		dataType *pointer = (Derived*)cBlock_->getObject();
	}


	unsigned int getRefCount() { return cBlock_->getRefCount(); }
	dataType *getObject() { return (dataType*)cBlock_->getObject(); }
private:

	//SmartPtr();

	CBlock *cBlock_;
};





template <typename dataType>
class EnableSmartFromThis
{
public:
	
	SmartPtr <dataType> SmartFromThis()
	{
		return smart_;
	}

	EnableSmartFromThis() : smart_()
	{

	}

	EnableSmartFromThis(const EnableSmartFromThis&)
	{
	}

	void setSmart(dataType *object)
	{
		smart_.setSmart(object);
	}
	/*
	void setSmart(dataType *smart)
	{
		smart_.EnableSmart(smart);
	}*/

protected:

	

	

	EnableSmartFromThis& operator=(EnableSmartFromThis&)
	{
		return (*this);
	}

	~EnableSmartFromThis()
	{
	}


private:

	SmartPtr <dataType> smart_;

	/*
	template<typename dataType1, typename dataType2>
		friend void _Do_enable(
			dataType1 *,
			EnableSmartFromThis<dataType2>*,
			_Ref_count_base *);

	
	*/
};

/*
template<typename dataType1, typename dataType2>
friend void _Do_enable(
	dataType1 *ptr,
	EnableSmartFromThis<dataType2>*Es,
	_Ref_count_base *refptr)
{	// reset internal weak pointer
	_Es->_Wptr._Resetw(_Ptr, _Refptr);
}
*/


#endif
