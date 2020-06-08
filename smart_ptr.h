#ifndef SMART_PTR_H
#define SMART_PTR_H

#include <iostream>

class CBlock
{
public:
	CBlock(void *object) : refCount_(1), object_(object) { std::cout << "new cBlock" << std::endl; }
	CBlock() : refCount_(0), object_(0) { std::cout << "new cBlock" << std::endl; }
	~CBlock() { std::cout << "delete cBlock" << std::endl; }
		
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
	friend class EnableSmartFromThis<dataType>;

	// needed, so that derived class pointer can be assigned to base class pointer
	template <class Other>
	friend class SmartPtr;

public:
	
	SmartPtr(dataType *object) : thisIsTheBottomSmartPointer_(false)
	{
		std::cout << "new smart ptr" << std::endl;

		cBlock_ = new CBlock(object);

		EnableSmart(object);
	}

	

	
	// this is called when the object is derived
	//from 'EnableSmartFromThis<dataType>'
	void EnableSmart(EnableSmartFromThis<dataType> *object)
	{
		std::cout << "template function" << std::endl;
		thereIsABottomSmartPointer_ = true;
		object->setSmart(cBlock_);
	}

	void EnableSmart(void *object)
	{
		thereIsABottomSmartPointer_ = false;

		std::cout << "do nothing function" << std::endl;
	}


	~SmartPtr()
	{
		std::cout << "delete smart pointer" << std::endl;

		if (!cBlock_) return;
		
		
		// in the case of the enableSmartFromThis - bottom smartPtr
		// don't delete the object, it's already being deleted.  the cBlock
		// will be deleted after the destruction of the bottom smartPtr, so it's
		// still safe to access cBlock at this point
		if (cBlock_->getRefCount() > 0)
		{
			cBlock_->decreaseRefCount();

			std::cout << "refcount: " << cBlock_->getRefCount() << std::endl;


			// if this the last smartPtr to the object (other than the bottom smartPtr),
			// delete the object if it still exists
			if (cBlock_->getRefCount() == 0)
			{


				if (cBlock_->getObject())
				{
					std::cout << "delete actual object" << std::endl;

					delete ((dataType*)cBlock_->getObject());
				}

				delete cBlock_;

			}
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

			// TODO: what to do about the enableSmartFromThis smartPtr
			// when reassigning ???
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
		std::cout << "new smart ptr" << std::endl;
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

		// checking line
		dataType *pointer = (Derived*)cBlock_->getObject();

		return *this;
	}


	// copy constructor
	template <typename Derived>
	SmartPtr(const SmartPtr<Derived> &other)
	{
		std::cout << "new smart ptr" << std::endl;
		std::cout << "copy constructor derived" << std::endl;
		cBlock_ = other.cBlock_;

		cBlock_->increaseRefCount();

		// checking line
		dataType *pointer = (Derived*)cBlock_->getObject();
	}


	unsigned int getRefCount() { return cBlock_->getRefCount(); }
	dataType *getObject() { return (dataType*)cBlock_->getObject(); }

private:
	// used to initialize the internal SmartPtr in 'EnableSmartFromThis'
	SmartPtr() : thisIsTheBottomSmartPointer_(true), thereIsABottomSmartPointer_(false), cBlock_(0) { std::cout << "new smart ptr" << std::endl; }

	void setCBlock(CBlock *cBlock)
	{
		cBlock_ = cBlock;

		//cBlock_->increaseRefCount();

	}

	//SmartPtr();

	bool thisIsTheBottomSmartPointer_;
	bool thereIsABottomSmartPointer_;
	CBlock *cBlock_;
};





template <typename dataType>
class EnableSmartFromThis
{
	friend class SmartPtr<dataType>;

public:
	
	

	EnableSmartFromThis() : smart_()
	{
		std::cout << "new enableSmartFromThis" << std::endl;
	}

	EnableSmartFromThis(const EnableSmartFromThis&)
	{
	}
			

protected:

	

	SmartPtr <dataType> SmartFromThis()
	{
		return smart_;
	}


	EnableSmartFromThis& operator=(EnableSmartFromThis&)
	{
		return (*this);
	}

	~EnableSmartFromThis()
	{
		std::cout << "delete enableSmartFromThis " << std::endl;
	}


private:

	

	void setSmart(CBlock *cBlock)
	{
		smart_.setCBlock(cBlock);
	}

	SmartPtr <dataType> smart_;

};



#endif
