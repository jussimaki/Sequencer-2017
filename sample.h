#ifndef SAMPLE_H
#define SAMPLE_H

#include <string>

#include "smart_ptr.h"
#include "callback_registerer.h"

enum SAMPLE_EVENT_TYPE {SAMPLE_NO_ACTION, SAMPLE_XXX_CHANGED};

class Sample;

class SampleEvent
{
public:
	SAMPLE_EVENT_TYPE type_;
	SmartPtr <Sample> sample_;
};


class Sample : public EnableSmartFromThis<Sample>
{
	

public:
	Sample(CallbackConcreteRegisterer<SampleEvent> *registerer) : registerer_(registerer), name_(""), data_(0), length_(0), sampleRate_(0) { std::cout << "new sample" << std::endl; }
	~Sample() { std::cout << "delete sample" << std::endl;  release(); }

		
	void copyData(signed short *data, unsigned int length);

	void setName(std::string name) { name_ = name; }
	void setData(signed short *data) { data_ = data; }
	void setLength(unsigned int length) { length_ = length; }
	void setSampleRate(unsigned int sampleRate) { sampleRate_ = sampleRate; }
	void release();
	SmartPtr<Sample> returnThis() { return SmartFromThis(); }
	std::string getName() { return name_; }
	signed short *getData() { return data_; }
	unsigned int getLength() { return length_; }
	unsigned int getSampleRate() { return sampleRate_; }




private:
	CallbackConcreteRegisterer<SampleEvent> *registerer_;
	std::string name_;
	signed short *data_;
	unsigned int length_;
	unsigned int sampleRate_;
};

#endif