#include "instrument_sample_model.h"

std::string InstrumentSampleModel::getInstrumentTypeName()
{
	return instrumentTypeName_;
}


std::string InstrumentSampleModel::getSampleDataTypeName()
{
	return sampleDataTypeName_;
}
std::string InstrumentSampleModel::getSampleTypeName()
{
	return sampleTypeName_;
}


void InstrumentSampleModel::addInstrument(Reference <Instrument> instrument)
{
	instruments_.push_back(instrument);

	notify(CONTAINER_CHANGE(instrumentTypeName_, CONTAINER_ACTION::CONTAINER_ADD, instruments_.size() - 1));
}

void InstrumentSampleModel::addSample(Reference <Sample> sample)
{
	samples_.push_back(sample);

	notify(CONTAINER_CHANGE(sampleTypeName_, CONTAINER_ACTION::CONTAINER_ADD, samples_.size() - 1));
}


// Container interface

Container * InstrumentSampleModel::getChildContainer(std::string type, unsigned int index)
{
	if (type == instrumentTypeName_)
	{
		if (index < instruments_.size())
		{
			return instruments_[index].acquireObject();
		}
		else return 0;
	}
	else return 0;
}

unsigned int InstrumentSampleModel::getItemAmount(std::string type)
{
	if (type == instrumentTypeName_)
	{
		return instruments_.size();
	}
	else if (type == sampleTypeName_)
	{
		return samples_.size();
	}
	else return 0;
}
