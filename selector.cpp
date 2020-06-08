#include <iostream>

#include "selector.h"

Selector::Selector(CallbackConcreteRegisterer <ContainerChangeEvent> *registerer, Selector *parent, SmartPtr <Container> container, std::string type) : registerer_(registerer), parent_(parent), container_(container), type_(type)
{
	registerer_->registerListener(this, Selector::callback_);

	if (parent_)
	{
		parent_->addChild(this);
	}
}

bool Selector::callback(ContainerChangeEvent *event)
{

	if(event->container_ == container_.getObject())
	std::cout << "container changed" << std::endl;

	return true;
}

bool Selector::callback_(void *me, ContainerChangeEvent *event)
{
	return ((Selector*)me)->callback(event);
}

std::string Selector::getType()
{
	return type_;
}

bool Selector::isSelected()
{
	return selected_;
}

unsigned int Selector::getSelectedIndex()
{
	return index_;
}

void Selector::addChild(Selector *child)
{
	children_.push_back(child);
}