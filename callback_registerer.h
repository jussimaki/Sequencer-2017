#ifndef CALLBACK_REGISTERER_H
#define CALLBACK_REGISTERER_H

#include <iostream>

#include "pool.h"
#include "event_notifier.h"

class CallbackRegisterer;
/*
class CallbackObject
{
public:
	virtual void callCallback() = 0;
	void *object_;
	CallbackRegisterer *registerer_;
};

template <typename dataType>
class ConcreteCallbackObject : public CallbackObject
{
public:

	void callCallback()
	{
		functionPointer_(object_, parameterData_);
	}

	
	void(*functionPointer_)(void *, dataType*);
	dataType *parameterData_;

};
*/


class CallbackRegisterer
{
public:
	CallbackRegisterer(EventNotifier *notifier/*ClassIDIssuer *issuer, std::string name*/) :// name_(name),
		notifier_(notifier)
	//	,ID_(issuer->issue(name))
	{ notifier_->addRegisterer(this); }
	
	//virtual void callCallbacks() = 0;
	//virtual void notify(void *data) = 0;
	
	//long int getID() { return ID_; }

	virtual bool convertAndCallCallback(bool(*functionPointer)(), void *object, void *data) = 0;
	virtual void deleteObject(void *object) = 0;
	//virtual CallbackObject *detachCallbackObject() = 0;
	//virtual void *retachCallbackObject() = 0;
	//virtual void returnInstance(CallbackInstance *instance) = 0;

protected:
	//std::string name_;
	EventNotifier *notifier_;
	//const long int ID_;
};

template <typename dataType>
struct ObjectFunctionPair
{
	void *object;
	bool(*function)(void *, dataType*);
};

template <typename dataType>
class CallbackConcreteRegisterer : public CallbackRegisterer
{
private:
	std::vector <ObjectFunctionPair<dataType>> pairs_;
	
	//Pool <ConcreteCallbackInstance<dataType>*> callbackPool_;
	//Pool <ConcreteCallbackInstance<dataType>*> emptyNodes_;

	//const unsigned int amountOfInstanceNodesToBeAllocated_;


public:
	CallbackConcreteRegisterer(EventNotifier *notifier/*, ClassIDIssuer *issuer, std::string name*/) : CallbackRegisterer(notifier)//, issuer, name)
	{}
	/*
	{
	
		for (unsigned int index = 0; index < amountOfInstanceNodesToBeAllocated_; index++)
		{
			Node <ConcreteCallbackInstance<dataType>*> *node = new Node <ConcreteCallbackInstance<dataType>*>;

			node->data = new ConcreteCallbackInstance<dataType>(this);
			node->last = 0;
			node->next = 0;

			callbackPool_.addToBegin(node);


		}

	}
	*/

	~CallbackConcreteRegisterer()
	{
		//std::cout << "registerer " << name_ << " destructor" << std::endl;
		notifier_->removeRegisterer(this);

		//callbackPool_.free();
		//emptyNodes_.clear();
	}

	bool convertAndCallCallback(bool(*functionPointer)(), void *object, void *data)
	{
		bool(*convertedPointer)(void *, dataType*) = (bool(*)(void *, dataType*))functionPointer;

		return convertedPointer(object, (dataType*)data);
	}
	/*
	void addEvent(Node <dataType *> *node)
	{
		events_.addToBegin(node);
	}
	*/
	
	void registerListener(void *object, bool(*function)(void *, dataType*))
	{
		ObjectFunctionPair <dataType> pair;

		pair.object = object;
		pair.function = function;
		

		pairs_.push_back(pair);
	}

	void unregisterListener(void *object)
	{
		for (unsigned int index = 0; index < pairs_.size(); index++)
		{
			if (pairs_[index].object == object)
			{
				pairs_.erase(pairs_.begin() + index);
				return;
			}
		}
	}

	void notify(dataType *data)
	{

		for (unsigned int index = 0; index < pairs_.size(); index++)
		{
			Node <CallbackInstance*> *node = notifier_->getBlankNode();

			node->last = 0;
			node->next = 0;


			CallbackInstance *instance = node->data;

			// fill instance data

			instance->registerer_ = this;

			instance->functionPointer_ = (bool(*)(void))pairs_[index].function;
			instance->object_ = pairs_[index].object;
			instance->data_ = data;

			notifier_->addOccurredCallback(node);
		}
	}
	/*
	void callCallback(void *object, void *data)
	{
		for (unsigned int index = 0; index < pairs_.size(); index++)
		{
			if (pairs_[index].object == object)
			{
				pairs_[index].function(object, (dataType*)data);
			}
		}
	}*/

	void deleteObject(void *object)
	{
		dataType *objectOfDataType = (dataType *)object;
		//std::cout << "delete: " << object << std::endl;
		delete objectOfDataType;
	}

	/*
	void returnInstance(CallbackInstance *instance)
	{
		if (instance->registerer_ != this) return;

		//Node <ConcreteCallbackInstance<dataType>*> *node = emptyNodes_.extractFirstNode();

		Node <CallbackInstance*> *node = notifier_->getEmptyNode();


		node->last = 0;
		node->next = 0;
				
		//node->data = (ConcreteCallbackInstance<dataType>*)instance;
		node->data = instance;

		node->data->functionPointer_ = 0;
		node->data->data_ = 0;
		node->data->object_ = 0;

		notifier_->

		//callbackPool_.addToBegin(node);
		
	}

	*/
};

#endif