#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <vector>
#include <iostream>

#include "sample.h"
#include "container.h"
#include "callback_registerer.h"
#include "smart_ptr.h"



class Instrument;
class InstrumentSampleModel;
class SampleData
{
public:
	SampleData(std::string name, SmartPtr <Sample> sample) :
		sample_(sample), name_(name),
		rootNote_(0), lowNote_(0), highNote_(127), loopType_(0), attack_(0.0001), decay_(0.1), sustain_(1), release_(0.01), 
		loopStart_(0), loopEnd_(0), poly_(true), portamento_(false), portamentoRate_(1.1)
	{
		loopStart_ = 0;

		loopEnd_ = sample.getObject()->getLength() - 1;
	}

	SmartPtr <Sample> getSample() { return sample_; }
	std::string getName() { return name_; }

	int getRootNote() { return rootNote_; }
	int getLowNote() { return lowNote_; }
	int getHighNote() { return highNote_; }

	int loopType() { return loopType_; }

	double getAttack() { return attack_; }
	double getDecay() { return decay_; }
	double getSustain() { return sustain_; }
	double getRelease() { return release_; }

	int getLoopStart() { return loopStart_; }
	int getLoopEnd() { return loopEnd_; }

	bool getPoly() { return poly_; }
	bool getPortamento() { return portamento_; }

	double getPortamentoRate() { return portamentoRate_; }

	void setRootNote(int note);
	void setLowNote(int note);
	void setHighNote(int note);

	

private:

	SampleData();
	
	SmartPtr <Sample> sample_;
	std::string name_;
	
	int rootNote_, lowNote_, highNote_;
	int loopType_;

	double attack_, decay_, sustain_, release_;
	int loopStart_, loopEnd_;

	bool poly_, portamento_;
	double portamentoRate_;

	

};




enum INSTRUMENT_TYPE {INSTRUMENT_MELODY, INSTRUMENT_DRUMS};


class Instrument : public Container, public EnableSmartFromThis<Instrument>
{
private:
	const std::string sampleDataTypeName_;
	std::vector <SmartPtr <SampleData>> sampleData_;
	std::string name_;
	INSTRUMENT_TYPE type_;

	
	Instrument();

public:

	Instrument(CallbackConcreteRegisterer <ContainerChangeEvent> *registerer, std::string name, INSTRUMENT_TYPE type) : Container(registerer), sampleDataTypeName_("SampleData"), name_(name), type_(type) { std::cout << "new instrument" << std::endl; }
	~Instrument() { release(); std::cout << "delete instrument" << std::endl; }
		
	
	void release() { clearSampleData(); }

	SmartPtr <SampleData> getSampleData(unsigned int index)
	{
		if (sampleData_.size() > index)
		{
			return sampleData_[index];
		}
		else return 0;
	}

	void addSampleData(std::string name, SmartPtr <Sample> sample)
	{
		SmartPtr <SampleData> newSampleData(new SampleData(name, sample));
		sampleData_.push_back(newSampleData);

		notify(CONTAINER_CHANGE(sampleDataTypeName_, CONTAINER_ACTION::CONTAINER_ADD, sampleData_.size() - 1));	
	}

	void deleteSampleData(unsigned int index)
	{
		if (sampleData_.size() > index)
			sampleData_.erase(sampleData_.begin() + index);

		notify(CONTAINER_CHANGE(sampleDataTypeName_, CONTAINER_ACTION::CONTAINER_ERASE, index));
	}

	unsigned int getSampleDataAmount()
	{
		return sampleData_.size();
	}

	void clearSampleData()
	{
		for (unsigned int index = 0; index < sampleData_.size(); index++)
		{
			sampleData_[index].deleteObject();
		}

		sampleData_.clear();
		
		notify(CONTAINER_CHANGE(sampleDataTypeName_, CONTAINER_ACTION::CONTAINER_CLEAR, 0));
	}


	//std::vector <SampleData> *getSampleData() { return &sampleData_; }
	
	
	
	void setName(std::string name) { name_ = name; }
	void setType(INSTRUMENT_TYPE type) { type_ = type; }

	std::string getName() { return name_; }
	INSTRUMENT_TYPE getType() { return type_; }

	std::string getSampleDataTypeName() { return sampleDataTypeName_; }

	// Container interface

	Container *getChildContainer(std::string type, unsigned int index) { return 0; }
	unsigned int getItemAmount(std::string type);
	


};

#endif