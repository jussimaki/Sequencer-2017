#ifndef INSTRUMENT_SETTINGS_MODEL_H
#define INSTRUMENT_SETTINGS_MODEL_H

#include "id.h"
#include "lock_hierarchy.h"
#include "callback_registerer.h"
#include "instrument_sample_model.h"
#include "subject.h"

#include "file_window_model.h"


enum SAMPLEDATA_CHANGE_MEMBER{rootNote, highNote, lowNote, loopType, attack, decay, sustain, release, loopStart, loopEnd, poly, portamento, portamentoRate};

class Instrument;
class EventNotifier;
class FileWindowModel;

class InstrumentSettingsModelGUIStuff
{
public:
	InstrumentSettingsModelGUIStuff(IDIssuer <SampleData> *sampleDataIDIssuer, IDIssuer <Sample> *sampleIDIssuer) :
		newInstrumentName_(""), newInstrumentType_(INSTRUMENT_TYPE::INSTRUMENT_MELODY),
		temporaryInstrumentName_("TEMPORARY"), temporaryInstrumentType_(INSTRUMENT_TYPE::INSTRUMENT_MELODY),
		instrumentSelected_(false), currentInstrumentIndex_(0), nextInstrumentIndex_(0),
		sampleDataSelected_(false), currentSampleDataIndex_(0), nextSampleDataIndex_(0),
		sampleSelected_(false), currentSampleIndex_(0), nextSampleIndex_(0),
		temporarySampleData_(sampleDataIDIssuer, sampleIDIssuer)
		{}

		
	std::string newInstrumentName_;
	INSTRUMENT_TYPE newInstrumentType_;

	std::string temporaryInstrumentName_;
	INSTRUMENT_TYPE temporaryInstrumentType_;

	bool instrumentSelected_;
	unsigned int currentInstrumentIndex_;
	unsigned int nextInstrumentIndex_;

	bool sampleDataSelected_;
	unsigned int currentSampleDataIndex_;
	unsigned int nextSampleDataIndex_;

	bool sampleSelected_;
	unsigned int currentSampleIndex_;
	unsigned int nextSampleIndex_;

	SampleData temporarySampleData_;
};


class InstrumentSettingsModelInternalMessage
{
public:
	InstrumentSettingsModelInternalMessage(IDIssuer <Instrument> *instrumentIDIssuer, IDIssuer <SampleData> *sampleDataIDIssuer, IDIssuer <Sample> *sampleIDIssuer):
		SUCCESS(false),
		INSTRUMENT_NAMES_READ(false),
		SAMPLE_NAMES_READ(false),
		TEMPORARY_INSTRUMENT_READ(false),
		TEMPORARY_INSTRUMENT_CHANGED(false),

		INSTRUMENT_SELECTED(false),
		INSTRUMENT_DESELECTED(false),
		INSTRUMENT_CHANGED(false),
		SELECTED_INSTRUMENT_DELETED(false),

		SAMPLE_SELECTED(false),
		SAMPLE_DESELECTED(false),
		SELECTED_SAMPLE_DELETED(false),

		SAMPLEDATA_SELECTED(false),
		SAMPLEDATA_DESELECTED(false),
		SELECTED_SAMPLEDATA_DELETED(false),
		

		index(0),
		fetched_(instrumentIDIssuer, "", INSTRUMENT_TYPE::INSTRUMENT_MELODY)
	//	sampleData_(sampleDataIDIssuer, sampleIDIssuer)
	{}

	bool SUCCESS, INSTRUMENT_NAMES_READ, SAMPLE_NAMES_READ, TEMPORARY_INSTRUMENT_READ, TEMPORARY_INSTRUMENT_CHANGED, INSTRUMENT_SELECTED, INSTRUMENT_DESELECTED,
		INSTRUMENT_CHANGED, SELECTED_INSTRUMENT_DELETED, SAMPLE_SELECTED, SAMPLE_DESELECTED, SELECTED_SAMPLE_DELETED, SAMPLEDATA_SELECTED,
		SAMPLEDATA_DESELECTED, SELECTED_SAMPLEDATA_DELETED;

	unsigned int index;

	Instrument fetched_;
};


class InstrumentSettingsModel
{
public:
	InstrumentSettingsModel(CASLock *eventLock, CASLock *coutLock, CASLock *fileLock, CallbackConcreteRegisterer<InstrumentSettingsModelInternalMessage> *instrumentSettingsModelInternalRegisterer,
		CallbackConcreteRegisterer <InstrumentSampleMessage> *instrumentSampleRegisterer, CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer,
		IDIssuer <Instrument> *instrumentIDIssuer, IDIssuer <SampleData> *sampleDataIDIssuer, IDIssuer <Sample> *sampleIDIssuer, InstrumentSampleModel *instrumentSampleModel,
		CASLock *instrumentSettingsModelLock, CASLock *instrumentSampleLock);

	~InstrumentSettingsModel();

	void registerObserver(Observer *observer);
	void unregisterObserver(Observer *observer);


	// GUI thread callbacks
	bool instrumentSampleCallback(InstrumentSampleMessage *message);
	bool static instrumentSampleCallback_(void *model, InstrumentSampleMessage *message);
	
	bool internalCallback(InstrumentSettingsModelInternalMessage *message);
	bool static internalCallback_(void *model, InstrumentSettingsModelInternalMessage *message);

	bool sampleFileChosenCallback(FileWindowMessage *message);
	bool static sampleFileChosenCallback_(void *model, FileWindowMessage *message);


	// GUI thread functions
	// called by controller and view
	// (so called interface functions?)
	void createNewInstrument();
	void deleteInstrument();
	void setTemporaryInstrumentName(std::string name);
	void setTemporaryInstrumentType(INSTRUMENT_TYPE type);
	void setNewInstrumentName(std::string name);
	void setNewInstrumentType(INSTRUMENT_TYPE type);
	void selectInstrument(unsigned int index);
	void selectSampleData(unsigned int index);
	void selectSample(unsigned int index);
	void createNewSampleData();

	void setRootNote(int rootNote);
	void setLowNote(int lowNote);
	void setHighNote(int highNote);
	void setLoopType(int type);

	int getRootNote();
	int getLowNote();
	int getHighNote();

	std::string getNewInstrumentName();
	INSTRUMENT_TYPE getNewInstrumentType();
	unsigned int getSelectedInstrumentIndex();
	bool instrumentSelected();
	unsigned int getSelectedSampleDataIndex();
	bool sampleDataSelected();
	unsigned int getSelectedSampleIndex();
	bool sampleSelected();

	InstrumentSettingsModelGUIStuff *getGUIStuff();
	

	// workers that are started in the interface functions
	void workerCreateNewInstrument(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerCreateNewInstrument_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);

	void workerDeleteInstrument(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerDeleteInstrument_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);
		

	void workerSelectInstrument(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerSelectInstrument_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);

	void workerSelectSampleData(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerSelectSampleData_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);

	void workerSelectSample(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerSelectSample_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);
	
	void workerCreateNewSampleData(InstrumentSettingsModelGUIStuff GUIStuff);
	void static workerCreateNewSampleData_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff);

	
	// workers that respond to changes in instrumentSampleModel_
	void workerInstrumentAdded();
	void static workerInstrumentAdded_(InstrumentSettingsModel *instrumentSettingsModel);

	void workerInstrumentChanged(ID <Instrument> ID);
	void static workerInstrumentChanged_(InstrumentSettingsModel *instrumentSettingsModel, ID <Instrument> ID);

	void workerInstrumentDeleted(ID <Instrument> ID);
	void static workerInstrumentDeleted_(InstrumentSettingsModel *instrumentSettingsModel, ID <Instrument> ID);

	void workerSampleAdded();
	void static workerSampleAdded_(InstrumentSettingsModel *instrumentSettingsModel);

	void workerSampleDeleted(InstrumentSettingsModelGUIStuff GUIStuff, ID <Sample> ID);
	void static workerSampleDeleted_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff, ID <Sample> ID);

	// worker that responds to file choosing event
	void workerLoadSample(std::string fileName);
	void static workerLoadSample_(InstrumentSettingsModel *instrumentSettingsModel, std::string fileName);


	// Data interface for outside workers.
	// These access the slow data.
	// instrumentSettingsModelLock_ must be acquired to call
	// these functions
	std::vector <std::string> getInstrumentNameList();
	std::vector <std::string> getSampleNameList();
	std::vector <std::string> getSampleDataNameList();
	Instrument getTemporaryInstrument();


	// worker thread functions.
	// instrumentSettingsModelLock_ must be acquired to call
	// these functions
	
	Sample getSampleDataSample();

	//std::string getSelectedInstrumentName();


private:

	// helper functions that are called by workers

	// NOTE: instrumentSettingsModelLock_ must be acquired to call the following functions!!
	void saveTemporaryInstrumentData(InstrumentSettingsModelGUIStuff GUIStuff);

	// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_
	// must be acquired to call the following functions!!
	
	void getTemporaryInstrumentByID(ID <Instrument> ID);
	void getInstrumentNames();
	void getSampleNames();
	void getSampleDataNames();
	bool getTemporaryInstrumentByIndex(unsigned int index);

	CASLock *eventLock_, *coutLock_, *fileLock_;

	CallbackConcreteRegisterer<InstrumentSettingsModelInternalMessage> *instrumentSettingsModelInternalRegisterer_;
	CallbackConcreteRegisterer <InstrumentSampleMessage> *instrumentSampleRegisterer_;
	CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer_;
	
	IDIssuer <Instrument> *instrumentIDIssuer_;
	IDIssuer <SampleData> *sampleDataIDIssuer_;
	IDIssuer <Sample> *sampleIDIssuer_;

	InstrumentSampleModel *instrumentSampleModel_;
	
	CASLock *instrumentSettingsModelLock_;
	CASLock *instrumentSampleLock_;
	

	// slow data protected by instrumentSettingsModelLock_
	// and manipulated in worker threads
	std::vector <std::string> instrumentNameList_;
	std::vector <std::string> sampleNameList_;
	std::vector <std::string> sampleDataNameList_;
	Instrument temporaryInstrument_; // this holds a copy of the chosen instrument
	Sample sampleDataSample_;
	//ID <Sample> selectedSampleID_;

	
	


	// data manipulated straight in GUI thread,
	// no locks involved.
	
	InstrumentSettingsModelGUIStuff GUIStuff_;

	Subject subject_;
};


#endif
