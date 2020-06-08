#ifndef SELECTOR_H
#define SELECTOR_H

#include <string>
#include <vector>

#include "reference.h"
#include "container.h"
#include "smart_ptr.h"

class ContainerChangeEvent;

class Selector
{
public:
	Selector(CallbackConcreteRegisterer <ContainerChangeEvent> *registerer, Selector *parent, SmartPtr <Container> container, std::string type);

	bool callback(ContainerChangeEvent *event);
	bool static callback_(void *me, ContainerChangeEvent *event);


	std::string getType();

	bool isSelected();
	unsigned int getSelectedIndex();

	void addChild(Selector *child);

private:
	Selector();

	CallbackConcreteRegisterer <ContainerChangeEvent> *registerer_;

	Selector *parent_;
	SmartPtr <Container> container_;

	std::string const type_;

	std::vector <Selector *> children_;

	bool selected_;
	unsigned int index_;

};

#endif
