#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
#include <iostream>

#include "callback_registerer.h"

enum CONTAINER_ACTION { NONE, CONTAINER_DESTRUCTION, CONTAINER_ADD, CONTAINER_ERASE, CONTAINER_CLEAR };

class CONTAINER_CHANGE
{
public:

	CONTAINER_CHANGE(std::string type, CONTAINER_ACTION action, unsigned int index) : type_(type), action_(action), index_(index) {};
	CONTAINER_CHANGE() : type_(""), action_(CONTAINER_ACTION::NONE), index_(0) {};

	void set(std::string type, CONTAINER_ACTION action, unsigned int index)
	{
		type_ = type;
		action_ = action;
		index_ = index;
	}

	void zero()
	{
		type_ = "";
		action_ = CONTAINER_ACTION::NONE;
		index_ = 0;
	}

	std::string getType() { return type_; }
	CONTAINER_ACTION getAction() { return action_; }
	unsigned int getIndex() { return index_; }

private:
	std::string type_;
	CONTAINER_ACTION action_;
	unsigned int index_;
};

class Container;

class ContainerChangeEvent
{
public:
	ContainerChangeEvent(CONTAINER_CHANGE change, Container *container) : change_(change), container_(container){}
	CONTAINER_CHANGE change_;
	Container *container_;
};

class Container
{
public:
	Container(CallbackConcreteRegisterer <ContainerChangeEvent> *registerer) : registerer_(registerer)
	{
		std::cout << "new container" << std::endl;
	}
	
	virtual ~Container()
	{
		std::cout << "delete container" << std::endl;
	}
	

	virtual unsigned int getItemAmount(std::string type) = 0;
	virtual Container *getChildContainer(std::string type, unsigned int index) = 0;
	void notify(CONTAINER_CHANGE change);


protected:
	CallbackConcreteRegisterer <ContainerChangeEvent> *registerer_;
	
};


#endif