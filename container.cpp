#include "container.h"

void Container::notify(CONTAINER_CHANGE change)
{
	registerer_->notify(new ContainerChangeEvent(change, this));
}