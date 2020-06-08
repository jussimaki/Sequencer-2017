#include <iostream>

#include "event_notifier.h"
#include "callback_registerer.h"


bool CallbackInstance::callCallback()
{
	if (registerer_)
	{
		return registerer_->convertAndCallCallback(functionPointer_, object_, data_);
	}
	else return false;
}

EventNotifier::EventNotifier(unsigned int emptyNodeAmount) : amountOfEmptyNodesToBeAllocated_(emptyNodeAmount)
{
	
	for (unsigned int index = 0; index < amountOfEmptyNodesToBeAllocated_; index++)
	{
		Node <CallbackInstance *> *node = new Node <CallbackInstance *>;


		node->data = new CallbackInstance;
		
		node->data->functionPointer_ = 0;
		node->data->object_ = 0;
		node->data->registerer_ = 0;

		node->next = 0;
		node->last = 0;
		
		nodePool_.addToBegin(node);
	}
	/*
	for (unsigned int index = 0; index < amountOfObjectNodesToBeAllocated_; index++)
	{
		Node <void *> *node = new Node <void *>;

		node->data = 0;
		node->last = 0;
		node->next = 0;

		objectPool_.addToBegin(node);
	}*/
}

EventNotifier::~EventNotifier()
{
	std::cout << "notifier destruction" << std::endl;
	
	// callback instances should have been moved to their registerers before deleting event notifier
	
	
	occurredCallbacks_.clear();
	nodePool_.clear();
	emptyNodes_.clear();
	tryAgain_.clear();
}

CallbackInstance *EventNotifier::getNextCallbackInstance()
{
	Node <CallbackInstance*> *node = occurredCallbacks_.extractFirstNode();

	if (!node) return 0;


	// detach instance from the node
	CallbackInstance *instance = node->data;

	node->data = 0;
	node->last = 0;
	node->next = 0;

	emptyNodes_.addToBegin(node);

	return instance;
}

CallbackInstance *EventNotifier::getNextCallbackInstance(void *object)
{
	Node <CallbackInstance*> *node = occurredCallbacks_.getFirstNode();

	while (node)
	{

		CallbackInstance *instance = node->data;

		if (instance)
		{
			if (instance->object_ == object)
			{
				occurredCallbacks_.extractNode(node);

				node->data = 0;
				node->last = 0;
				node->next = 0;

				emptyNodes_.addToBegin(node);

				return instance;
			}
			else node = node->next;
		}
		else node = node->next;
	}

	return 0;
	
}

void EventNotifier::tryInstanceAgain(CallbackInstance *instance)
{
	Node <CallbackInstance*> *node = emptyNodes_.extractFirstNode();

	if (!node) return;

	node->data = instance;
	node->last = 0;
	node->next = 0;
	
	tryAgain_.addToBegin(node);
}

void EventNotifier::newRound()
{
	Node <CallbackInstance*> *node = 0;

	while (node = tryAgain_.extractLastNode())
	{
		occurredCallbacks_.addToBegin(node);
	}
}

// this is called after the instance's callback is handled
void EventNotifier::returnCallbackInstance(CallbackInstance *instance)
{
	// if there aren't any more occurred instances that use this allocated function argument
	if (!doesDataExist(instance->data_))
	{
		// then use the registerer to convert from void pointer
		// to the correct type and delete the argument
		instance->registerer_->deleteObject(instance->data_);
	}
	
	// zero the callback's function argument data
	instance->data_ = 0;

	instance->functionPointer_ = 0;
	instance->object_ = 0;
	instance->registerer_ = 0;
	
	
	//instance->registerer_->returnInstance(instance);

	Node <CallbackInstance*> *node = emptyNodes_.extractFirstNode();

	if (!node) return;

	node->data = instance;
	node->last = 0;
	node->next = 0;

	nodePool_.addToBegin(node);
}

void EventNotifier::callCallbacks()
{
	/*
	Node <CallbackInstance *> *instanceNode = occurredCallbacks_.getFirstNode();

	while (instanceNode)
	{
		// loop through the objects of the callback instance
		Node <void *> *objectNode = instanceNode->data->objects_.getFirstNode();
				
		while (objectNode)
		{
			// do the actual calling of callback
			instanceNode->data->registerer_->callCallback(objectNode->data, instanceNode->data->data_);
			objectNode = objectNode->next;
		}


		// empty the callable objects to the object pool
		instanceNode->data->objects_.emptyToAnotherPool(&objectPool_);
		
		// after all the objects have been called the callback parameter data can be deleted
		instanceNode->data->registerer_->deleteObject(instanceNode->data->data_);
		
		instanceNode = instanceNode->next;
	}

	occurredCallbacks_.emptyToAnotherPool(&instancePool_);
	*/
}


bool EventNotifier::doesDataExist(void *data)
{
	Node <CallbackInstance *> *instanceNode = occurredCallbacks_.getFirstNode();

	while (instanceNode)
	{
		if (instanceNode->data->data_ == data) return true;
		instanceNode = instanceNode->next;
	}

	return false;
}

void EventNotifier::addRegisterer(CallbackRegisterer *registerer)
{
	registerers_.push_back(registerer);
}




void EventNotifier::removeRegisterer(CallbackRegisterer *registerer)
{
	for (unsigned int index = 0; index < registerers_.size(); index++)
	{
		if (registerers_[index] == registerer)
		{
			registerers_.erase(registerers_.begin() + index);
			break;
		}
	}


	// remove all the callback instances that came from this registerer

	Node <CallbackInstance *> *instanceNode = occurredCallbacks_.getFirstNode();
	while (instanceNode)
	{
		Node <CallbackInstance *> *next = instanceNode->next;

		if (instanceNode->data->registerer_ == registerer)
		{
			occurredCallbacks_.extractNode(instanceNode);

			// detach the function parameter data from instance
			void *data = instanceNode->data->data_;
			instanceNode->data->data_ = 0;
			
			// if there are no more instances containing the data, delete it
			if (!doesDataExist(data)) instanceNode->data->registerer_->deleteObject(data);

			instanceNode->data->functionPointer_ = 0;
			instanceNode->data->object_ = 0;
			instanceNode->data->registerer_ = 0;


			
			// add the node to the empty node pool
			nodePool_.addToBegin(instanceNode);

		}
	

		instanceNode = next;
	}

}
void EventNotifier::addOccurredCallback(Node<CallbackInstance*> *instanceNode)
{
	instanceNode->last = 0;
	instanceNode->next = 0;
	occurredCallbacks_.addToEnd(instanceNode);

}
/*
void EventNotifier::addOccurredCallback(CallbackInstance *instance)
{

	if (!instance) return;
	Node <CallbackInstance*> *node = emptyNodePool_.extractFirstNode();

	node->data = instance;
	node->last = 0;
	node->next = 0;

	occurredCallbacks_.addToEnd(node);
}
*/

// returns a node that has no callback instance attached. this function is to be called when returning an instance
Node <CallbackInstance *>*EventNotifier::getEmptyNode()
{
	return emptyNodes_.extractFirstNode();
}

// returns a node that has a 'blank' callback instance that is to be filled with correct data.
// this function is to be called when creating a new callback instance
Node <CallbackInstance *>*EventNotifier::getBlankNode()
{
	return nodePool_.extractFirstNode();
}


bool EventNotifier::checkForCallbacks(void *object)
{
	Node <CallbackInstance *> *instanceNode = occurredCallbacks_.getFirstNode();

	while (instanceNode)
	{
		if (instanceNode->data)
		{
			if (instanceNode->data->object_ == object) return true;
		}

		instanceNode = instanceNode->next;
	}

	return false;
}