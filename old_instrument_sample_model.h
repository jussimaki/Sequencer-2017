#ifndef INSTRUMENT_SAMPLE_MODEL_H
#define INSTRUMENT_SAMPLE_MODEL_H

#include <vector>

#include "container.h"
#include "instrument.h"
#include "sample.h"

#include "reference.h"

class InstrumentSampleModel : public Container
{
public:
	InstrumentSampleModel(CallbackConcreteRegisterer <ContainerChangeEvent> *registerer, EntryHolder <Instrument> *instrumentHolder, EntryHolder <Sample> *sampleHolder) : Container(registerer),
		//instrumentIDAmount_(instrumentIDAmount), sampleIDAmount_(sampleIDAmount),
		instrumentTypeName_("Instrument"), sampleDataTypeName_("SampleData"), sampleTypeName_("Sample"),
		//instrumentIDIssuer_(instrumentIDAmount_),
		instrumentHolder_(instrumentHolder),
		sampleHolder_(sampleHolder)
	{}

	std::string getInstrumentTypeName();
	std::string getSampleDataTypeName();
	std::string getSampleTypeName();

	void addInstrument(Reference <Instrument> instrument);
	void addSample(Reference <Sample> sample);

	// Container interface

	Container *getChildContainer(std::string type, unsigned int index);
	unsigned int getItemAmount(std::string type);
	
private:

	//const unsigned int instrumentIDAmount_, sampleIDAmount_;
	const std::string instrumentTypeName_, sampleDataTypeName_, sampleTypeName_;

	//IDIssuer <Instrument> instrumentIDIssuer_;
	EntryHolder <Instrument> *instrumentHolder_;
	EntryHolder <Sample> *sampleHolder_;

	std::vector <Reference <Instrument>> instruments_;
	std::vector <Reference <Sample>> samples_;

	

	
};

#endif