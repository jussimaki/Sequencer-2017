#include "instrument.h"



void SampleData::setRootNote(int note)
{
	rootNote_ = note;
}

void SampleData::setLowNote(int note)
{
	lowNote_ = note;
}

void SampleData::setHighNote(int note)
{
	highNote_ = note;
}

unsigned int Instrument::getItemAmount(std::string type)
{
	if (type == sampleDataTypeName_)
	{
		return sampleData_.size();
	}
	else return 0;
}



