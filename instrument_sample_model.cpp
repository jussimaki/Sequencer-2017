#include "instrument_sample_model.h"
#include "lock_hierarchy.h"
#include "lock_guard.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;

InstrumentSampleModel::InstrumentSampleModel(CASLock *eventLock, CallbackConcreteRegisterer <InstrumentSampleMessage> *messageRegisterer) :
	eventLock_(eventLock), messageRegisterer_(messageRegisterer)
{}

Instrument::Instrument(IDIssuer <Instrument> *instrumentIDIssuer, std::string name, INSTRUMENT_TYPE type) : ID_(instrumentIDIssuer), name_(name), type_(type)
{
	ID_.acquireID();
}


ID <Instrument> Instrument::getID()
{
	return ID_;
}

void Instrument::addSampleData(SampleData data)
{
	bool found = false;

	for (unsigned int index = 0; index < sampleData_.size(); index++)
	{
		if (sampleData_[index].getSampleDataID()  == data.getSampleDataID())
		{
			sampleData_[index] = data;
			found = true;
			break;
		}
	}

	if(!found) sampleData_.push_back(data);
}

unsigned int Instrument::getSampleDataAmount()
{
	return sampleData_.size();
}

SampleData *Instrument::getSampleData(ID <SampleData> ID)
{
	for (unsigned int index = 0; index < sampleData_.size(); index++)
	{
		if (ID == sampleData_[index].getSampleDataID())
			return &sampleData_[index];
	}

	return 0;
}

std::string Instrument::getName()
{
	return name_;
}

INSTRUMENT_TYPE Instrument::getType()
{
	return type_;
}

void Instrument::setName(std::string name)
{
	name_ = name;
}

void Instrument::setType(INSTRUMENT_TYPE type)
{
	type_ = type;
}

void Instrument::returnID()
{
	ID_.returnID();
}

// adds sampleData to an instrument
void InstrumentSampleModel::addSampleData(ID <Instrument> instrumentID, SampleData data)
{
	bool found = false;

	for (unsigned int index = 0; index < instruments_.size(); index++)
	{
		if (instruments_[index]->getID() == instrumentID)
		{
			found = true;

			instruments_[index]->addSampleData(data);
			break;
		}
	}

	if (found)
	{
		InstrumentSampleMessage *message = new InstrumentSampleMessage(InstrumentSampleMessage::MessageType::CHANGE_INSTRUMENT, instrumentID, 0);

		LockGuard guard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
		messageRegisterer_->notify(message);
	}
}

void InstrumentSampleModel::addSample(Sample *sample)
{
	samples_.push_back(sample);

	InstrumentSampleMessage *message = new InstrumentSampleMessage(InstrumentSampleMessage::MessageType::ADD_SAMPLE, 0, sample->getID());

	LockGuard guard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

	messageRegisterer_->notify(message);

}

void InstrumentSampleModel::addInstrument(Instrument *instrument)
{
	instruments_.push_back(instrument);

	InstrumentSampleMessage *message = new InstrumentSampleMessage(InstrumentSampleMessage::MessageType::ADD_INSTRUMENT, instrument->getID(), 0);

	LockGuard guard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

	messageRegisterer_->notify(message);

}

void InstrumentSampleModel::deleteInstrument(unsigned int index)
{
	ID <Instrument> ID = instruments_[index]->getID();

	delete instruments_[index];
	instruments_.erase(instruments_.begin() + index);

	InstrumentSampleMessage *message = new InstrumentSampleMessage(InstrumentSampleMessage::MessageType::DELETE_INSTRUMENT, ID, 0);

	LockGuard guard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	messageRegisterer_->notify(message);
}


unsigned int InstrumentSampleModel::getInstrumentAmount()
{
	return instruments_.size();
}

Instrument *InstrumentSampleModel::getInstrument(unsigned int index)
{
	if (instruments_.size() > index) return instruments_[index];
	else return 0;
}

Instrument *InstrumentSampleModel::getInstrument(ID <Instrument> ID)
{
	
	for (unsigned int index = 0; index < instruments_.size(); index++)
	{
		if (instruments_[index]->getID() == ID)
			return instruments_[index];
	}

	return 0;
}

//overwrites the instrument with a matching ID with given instrument
void InstrumentSampleModel::changeInstrument(Instrument instrument)
{
	bool success = false;

	for (unsigned int index = 0; index < instruments_.size(); index++)
	{
		if (instruments_[index]->getID() == instrument.getID())
		{
			success = true;
			*instruments_[index] = instrument;
			break;
		}
	}

	if (success)
	{
		InstrumentSampleMessage *message = new InstrumentSampleMessage(InstrumentSampleMessage::MessageType::CHANGE_INSTRUMENT, instrument.getID(), 0);

		LockGuard guard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
		messageRegisterer_->notify(message);
	}
}

unsigned int InstrumentSampleModel::getSampleAmount()
{
	return samples_.size();
}

Sample *InstrumentSampleModel::getSample(unsigned int index)
{
	if (samples_.size() > index) return samples_[index];
	else return 0;
}


ID <Sample> Sample::getID()
{
	return ID_;
}

unsigned int Sample::getLength()
{
	return length_;
}

std::string Sample::getName()
{
	return name_;
}

signed short *Sample::getData()
{
	return data_;
}