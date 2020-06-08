#include "subject.h"
#include "observer.h"

void Subject::notify()
{
	for (unsigned int index = 0; index < observers_.size(); index++)
		observers_[index]->update(this);
}

void Subject::clearObservers()
{
	observers_.clear();
}

void Subject::registerObserver(Observer *observer)
{
	for (unsigned int index = 0; index < observers_.size(); index++)
	{
		if (observers_[index] == observer) return;
	}

	observers_.push_back(observer);
}

void Subject::unregisterObserver(Observer *observer)
{
	for (unsigned int index = 0; index < observers_.size(); index++)
	{
		if (observers_[index] == observer)
		{
			observers_.erase(observers_.begin() + index);
			return;
		}
	}
}