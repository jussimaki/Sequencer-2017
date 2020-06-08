#ifndef EVENT_NOTIFIER_H
#define EVENT_NOTIFIER_H

#include <vector>
#include <string>
//#include <iostream>

#include "pool.h"


class CallbackRegisterer;

class CallbackInstance
{
public:
	CallbackInstance() : functionPointer_(0), object_(0), data_(0), registerer_(0) {}
	bool callCallback();

	bool(*functionPointer_)(); // this won't be called without conversion

	void *object_;
	void *data_;
	

	CallbackRegisterer *registerer_;


	
private:
	
};


class ClassIDIssuer
{
public:
	ClassIDIssuer() : counter_(0) {}

	long int issue(std::string name)
	{
		for (long int index = 0; index < names_.size(); index++)
		{
			if (name == names_[index]) return index;
		}
		counter_++;
		names_.push_back(name);

		return counter_;
	}

	
private:
	long int counter_;
	std::vector <std::string> names_;

};


class EventNotifier
{
private:
	std::vector <CallbackRegisterer *> registerers_;

	Pool <CallbackInstance *> occurredCallbacks_;
	Pool <CallbackInstance *> tryAgain_;
	Pool <CallbackInstance *> nodePool_;
	Pool <CallbackInstance *> emptyNodes_;


	//Pool <void *> objectPool_;



	const unsigned int amountOfEmptyNodesToBeAllocated_;

	EventNotifier();
public:

	EventNotifier(unsigned int emptyNodeAmount);
	~EventNotifier();
	

	bool doesDataExist(void *data);
	void callCallbacks();


	void addRegisterer(CallbackRegisterer *registerer);
	void removeRegisterer(CallbackRegisterer *registerer);

	void addOccurredCallback(Node<CallbackInstance*>* instanceNode);
	
	Node <CallbackInstance *>*getEmptyNode();
	Node <CallbackInstance *>*getBlankNode();

	bool checkForCallbacks(void *object);
	CallbackInstance *getNextCallbackInstance();
	CallbackInstance *getNextCallbackInstance(void *object);

	void tryInstanceAgain(CallbackInstance *instance);
	void newRound();
	void returnCallbackInstance(CallbackInstance *instance);
	

};




#endif