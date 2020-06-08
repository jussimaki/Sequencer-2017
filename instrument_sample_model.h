#ifndef INSTRUMENT_SAMPLE_MODEL_H
#define INSTRUMENT_SAMPLE_MODEL_H

#include <vector>

#include "id.h"
#include "callback_registerer.h"

class Instrument;
class Sample;

class InstrumentSampleMessage
{
public:
	enum MessageType { ADD_INSTRUMENT, DELETE_INSTRUMENT, CHANGE_INSTRUMENT, ADD_SAMPLE, DELETE_SAMPLE };

	InstrumentSampleMessage(InstrumentSampleMessage::MessageType type, ID <Instrument> instrumentID, ID <Sample> sampleID) :
		type_(type), instrumentID_(instrumentID), sampleID_(sampleID)
	{

	}
	

	MessageType type_;
	ID <Instrument> instrumentID_;
	ID <Sample> sampleID_;
};

class Sample
{
public:
	Sample(IDIssuer <Sample> *sampleIDIssuer, std::string name): data_(0), length_(0), sampleRate_(1), name_(name), ID_(sampleIDIssuer)
	{
		ID_.acquireID();
	}

	~Sample()
	{
		if (data_) {
			delete[] data_; data_ = 0;
		}
	}

	Sample(const Sample &other) : data_(0), length_(other.length_), sampleRate_(other.sampleRate_), name_(other.name_), ID_(other.ID_)
	{

		
		if (other.data_)
		{
			
			data_ = new signed short[length_];
			memcpy(data_, other.data_, sizeof(signed short)*length_);
		}
	}

	Sample& operator=(const Sample& other)
	{
		if (data_) delete[] data_;
		data_ = 0;

		if (other.data_)
		{
			
			data_ = new signed short[other.length_];

			memcpy(data_, other.data_, sizeof(signed short)*other.length_);
		}

		length_ = other.length_;
		sampleRate_ = other.sampleRate_;
		name_ = other.name_;
		ID_ = other.ID_;

		return *this;
	}

	ID <Sample> getID();
	unsigned int getLength();
	std::string getName();
	signed short *getData();

	void setName(std::string name) { name_ = name; }
	void setData(signed short *data) { data_ = data; }
	void setLength(unsigned int length) { length_ = length; }
	void setSampleRate(unsigned int sampleRate) { sampleRate_ = sampleRate; }

private:
	signed short *data_;
	unsigned int length_;
	unsigned int sampleRate_;
	std::string name_;
	ID <Sample> ID_;
};


class SampleDataParameters
{
public:
	SampleDataParameters() : rootNote_(0), lowNote_(0), highNote_(127), loopType_(0), attack_(0.0001), decay_(0.1), sustain_(1), release_(0.01),
		loopStart_(0), loopEnd_(0), poly_(true), portamento_(false), portamentoRate_(1.1) {}

	int rootNote_, lowNote_, highNote_;
	int loopType_;

	double attack_, decay_, sustain_, release_;
	int loopStart_, loopEnd_;

	bool poly_, portamento_;
	double portamentoRate_;
};

class SampleData
{
public:
	
	SampleData(IDIssuer <SampleData> *sampleDataIDIssuer, Sample *sample) : sampleDataID_(sampleDataIDIssuer), sampleID_(sample->getID())
		
	{
		sampleDataID_.acquireID();

		parameters_.loopStart_ = 0;

		if (sample)
		{
			parameters_.loopEnd_ = sample->getLength() - 1;
		}
		else parameters_.loopEnd_ = 0;
	}

	SampleData(IDIssuer <SampleData> *sampleDataIDIssuer, IDIssuer <Sample> *sampleIDIssuer) : sampleDataID_(sampleDataIDIssuer), sampleID_(sampleIDIssuer)
	{
		sampleDataID_.acquireID();

		// should the sample ID be acquired like this?
		//sampleID_.acquireID();
	}

	ID<Sample> getSampleID() { return sampleID_; }
	ID<SampleData> getSampleDataID() { return sampleDataID_; }


	int getRootNote() { return parameters_.rootNote_; }
	int getLowNote() { return parameters_.lowNote_; }
	int getHighNote() { return parameters_.highNote_; }

	int getLoopType() { return parameters_.loopType_; }

	double getAttack() { return parameters_.attack_; }
	double getDecay() { return parameters_.decay_; }
	double getSustain() { return parameters_.sustain_; }
	double getRelease() { return parameters_.release_; }

	int getLoopStart() { return parameters_.loopStart_; }
	int getLoopEnd() { return parameters_.loopEnd_; }

	bool getPoly() { return parameters_.poly_; }
	bool getPortamento() { return parameters_.portamento_; }

	double getPortamentoRate() { return parameters_.portamentoRate_; }


	void setRootNote(int rootNote) { parameters_.rootNote_ = rootNote; }
	void setLowNote(int lowNote) { parameters_.lowNote_ = lowNote; }
	void setHighNote(int highNote) { parameters_.highNote_ = highNote; }

	void setLoopType(int loopType) { parameters_.loopType_ = loopType; }

	void setAttack(double attack) { parameters_.attack_ = attack; }
	void setDecay(double decay) { parameters_.decay_ = decay; }
	void setSustain(double sustain) { parameters_.sustain_ = sustain; }
	void setRelease(double release) { parameters_.release_ = release; }

	void setLoopStart(int loopStart) { parameters_.loopStart_ = loopStart; }
	void setLoopEnd(int loopEnd) { parameters_.loopEnd_ = loopEnd; }

	void setPoly(bool poly) { parameters_.poly_ = poly; }
	void setPortamento(bool portamento) { parameters_.portamento_ = portamento; }

	void setPortamentoRate(double portamentoRate) { parameters_.portamentoRate_ = portamentoRate; }


	void setParameters(SampleDataParameters parameters) { parameters_ = parameters; }
	SampleDataParameters getParameters() { return parameters_; }

private:
	ID <SampleData> sampleDataID_;
	ID <Sample> sampleID_;

	SampleDataParameters parameters_;
	
};

enum INSTRUMENT_TYPE { INSTRUMENT_MELODY, INSTRUMENT_DRUMS };

class Instrument
{
public:
	Instrument(IDIssuer <Instrument> *instrumentIDIssuer, std::string name, INSTRUMENT_TYPE type);
	/*
	Instrument& operator=(const Instrument &other)
	{
		ID_ = other.ID_;
		name_ = other.name_;
		type_ = other.type_;
		
		return *this;
	}
	*/

	ID <Instrument> getID();
	void addSampleData(SampleData data);
	unsigned int getSampleDataAmount();
	SampleData *getSampleData(unsigned int index) { return &sampleData_[index]; }
	SampleData *getSampleData(ID <SampleData> ID);
	std::string getName();
	INSTRUMENT_TYPE getType();

	void setName(std::string name);
	void setType(INSTRUMENT_TYPE type);

	void returnID();
private:
	ID <Instrument> ID_;

	std::vector <SampleData> sampleData_;
	std::string name_;
	INSTRUMENT_TYPE type_;
	
};

class CASLock;
class LockHierarchy;

class InstrumentSampleModel
{
public:
	InstrumentSampleModel(CASLock *eventLock, CallbackConcreteRegisterer <InstrumentSampleMessage> *messageRegisterer);


	void addSample(Sample *sample);
	void addSampleData(ID <Instrument> instrumentID, SampleData data);
	void addInstrument(Instrument *instrument);

	void deleteInstrument(unsigned int index);

	unsigned int getInstrumentAmount();
	Instrument *getInstrument(unsigned int index);
	Instrument *getInstrument(ID <Instrument> instrumentID);

	void changeInstrument(Instrument instrument);

	unsigned int getSampleAmount();
	Sample *getSample(unsigned int index);

private:

	CallbackConcreteRegisterer <InstrumentSampleMessage> *messageRegisterer_;
	
	CASLock *eventLock_;
	std::vector <Sample *> samples_;
	std::vector <Instrument *> instruments_;

};

#endif