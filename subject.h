#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
class Observer;

class Subject
{
public:
	void registerObserver(Observer *observer);
	void unregisterObserver(Observer *observer);
	void clearObservers();

	void notify();
protected:
	std::vector <Observer *> observers_;
};

#endif