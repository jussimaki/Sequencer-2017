#include "spawn_thread.h"
#include "instrument_settings_model.h"

InstrumentSettingsModel::InstrumentSettingsModel(CASLock *eventLock, CASLock *coutLock, CASLock *fileLock, CallbackConcreteRegisterer<InstrumentSettingsModelInternalMessage> *instrumentSettingsModelInternalRegisterer,
	CallbackConcreteRegisterer <InstrumentSampleMessage> *instrumentSampleRegisterer, CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer, IDIssuer <Instrument> *instrumentIDIssuer,
	IDIssuer <SampleData> *sampleDataIDIssuer, IDIssuer <Sample> *sampleIDIssuer,
	InstrumentSampleModel *instrumentSampleModel, CASLock *instrumentSettingsModelLock, CASLock *instrumentSampleLock) :
	eventLock_(eventLock), coutLock_(coutLock), fileLock_(fileLock),
	instrumentSettingsModelInternalRegisterer_(instrumentSettingsModelInternalRegisterer),
	instrumentSampleRegisterer_(instrumentSampleRegisterer),
	fileWindowMessageRegisterer_(fileWindowMessageRegisterer),
	instrumentIDIssuer_(instrumentIDIssuer),
	sampleDataIDIssuer_(sampleDataIDIssuer),
	sampleIDIssuer_(sampleIDIssuer),
	instrumentSampleModel_(instrumentSampleModel),
	instrumentSettingsModelLock_(instrumentSettingsModelLock),
	instrumentSampleLock_(instrumentSampleLock),
	temporaryInstrument_(instrumentIDIssuer_, "", INSTRUMENT_TYPE::INSTRUMENT_MELODY),
	sampleDataSample_(sampleIDIssuer_, ""),
	GUIStuff_(sampleDataIDIssuer_, sampleIDIssuer_)
		
{
	instrumentSettingsModelInternalRegisterer_->registerListener(this, &internalCallback_);
	instrumentSampleRegisterer_->registerListener(this, &instrumentSampleCallback_);


	fileWindowMessageRegisterer_->registerListener(this, sampleFileChosenCallback_);
}

InstrumentSettingsModel::~InstrumentSettingsModel()
{
	instrumentSampleRegisterer_->unregisterListener(this);
	instrumentSettingsModelInternalRegisterer_->unregisterListener(this);
}


// processes messages sent by instrumentSampleModel_
bool InstrumentSettingsModel::instrumentSampleCallback(InstrumentSampleMessage *message)
{
	COUNTED_THREAD worker;

	{
		LockGuard guard(&localLockHierarchy, &coutLock_, 1, LOCK_LEVELS::IO);
		std::cout << "callback " << message << std::endl;
	}

	switch (message->type_)
	{
	case InstrumentSampleMessage::MessageType::ADD_INSTRUMENT:
		worker = COUNTED_THREAD(InstrumentSettingsModel::workerInstrumentAdded_, this);
		break;

	case InstrumentSampleMessage::MessageType::CHANGE_INSTRUMENT:
		worker = COUNTED_THREAD(InstrumentSettingsModel::workerInstrumentChanged_, this, message->instrumentID_);
		break;
		
	case InstrumentSampleMessage::MessageType::DELETE_INSTRUMENT:
		worker = COUNTED_THREAD(InstrumentSettingsModel::workerInstrumentDeleted_, this, message->instrumentID_);
		break;

	case InstrumentSampleMessage::MessageType::ADD_SAMPLE:
		worker = COUNTED_THREAD(InstrumentSettingsModel::workerSampleAdded_, this);
		break;

	case InstrumentSampleMessage::MessageType::DELETE_SAMPLE:
		worker = COUNTED_THREAD(InstrumentSettingsModel::workerSampleDeleted_, this, GUIStuff_, message->sampleID_);
		break;

	default:
		break;

	}

	return true;
}


bool InstrumentSettingsModel::instrumentSampleCallback_(void *model, InstrumentSampleMessage *message)
{
	return ((InstrumentSettingsModel*)model)->instrumentSampleCallback(message);
}



bool InstrumentSettingsModel::internalCallback(InstrumentSettingsModelInternalMessage *message)
{
	if (message->INSTRUMENT_SELECTED)
	{
		if (!message->SUCCESS)
		{
			GUIStuff_.currentInstrumentIndex_ = 0;
			GUIStuff_.instrumentSelected_ = false;


		}
		else
		{
			GUIStuff_.currentInstrumentIndex_ = GUIStuff_.nextInstrumentIndex_;
			GUIStuff_.instrumentSelected_ = true;

			GUIStuff_.temporaryInstrumentName_ = message->fetched_.getName();
			GUIStuff_.temporaryInstrumentType_ = message->fetched_.getType();
		}
	}

	if (message->SAMPLEDATA_SELECTED)
	{
		if (!message->SUCCESS)
		{
			GUIStuff_.currentSampleDataIndex_ = 0;
			GUIStuff_.sampleDataSelected_ = false;
		}
		else
		{
			GUIStuff_.currentSampleDataIndex_ = message->index;
			GUIStuff_.sampleDataSelected_ = true;

			GUIStuff_.temporarySampleData_ = *message->fetched_.getSampleData(message->index);
		}
	}

	if (message->SAMPLEDATA_DESELECTED)
	{
		GUIStuff_.currentSampleDataIndex_ = 0;
		GUIStuff_.sampleDataSelected_ = false;
	}

	if (message->SAMPLE_SELECTED)
	{
		if (!message->SUCCESS)
		{
			GUIStuff_.currentSampleIndex_ = 0;
			GUIStuff_.sampleSelected_ = false;
		}
		else
		{
			GUIStuff_.currentSampleIndex_ = message->index;
			GUIStuff_.sampleSelected_ = true;
		}
	}


	subject_.notify();

	return true;
}


bool InstrumentSettingsModel::internalCallback_(void *model, InstrumentSettingsModelInternalMessage *message)
{
	return ((InstrumentSettingsModel*)model)->internalCallback(message);
}


bool InstrumentSettingsModel::sampleFileChosenCallback(FileWindowMessage *message)
{
	
		
	COUNTED_THREAD worker(workerLoadSample_, this, message->fileName_);
	//worker.detach();
		
	return true;
}

bool InstrumentSettingsModel::sampleFileChosenCallback_(void *model, FileWindowMessage *message)
{
	return ((InstrumentSettingsModel*)model)->sampleFileChosenCallback(message);
}


void InstrumentSettingsModel::createNewInstrument()
{
	COUNTED_THREAD worker(workerCreateNewInstrument_, this, GUIStuff_);
	//worker.detach();
}


void InstrumentSettingsModel::deleteInstrument()
{
	COUNTED_THREAD worker(workerDeleteInstrument_, this, GUIStuff_);
	//worker.detach();
}

void InstrumentSettingsModel::setTemporaryInstrumentName(std::string name)
{
	GUIStuff_.temporaryInstrumentName_ = name;
}

void InstrumentSettingsModel::setTemporaryInstrumentType(INSTRUMENT_TYPE type)
{
	GUIStuff_.temporaryInstrumentType_ = type;
}

void InstrumentSettingsModel::setNewInstrumentName(std::string name)
{
	GUIStuff_.newInstrumentName_ = name;
}

void InstrumentSettingsModel::setNewInstrumentType(INSTRUMENT_TYPE type)
{
	GUIStuff_.newInstrumentType_ = type;
}



void InstrumentSettingsModel::selectInstrument(unsigned int index)
{
	GUIStuff_.nextInstrumentIndex_ = index;
	COUNTED_THREAD worker(workerSelectInstrument_, this, GUIStuff_);
	
}

void InstrumentSettingsModel::selectSampleData(unsigned int index)
{
	GUIStuff_.nextSampleDataIndex_ = index;

	COUNTED_THREAD worker(workerSelectSampleData_, this, GUIStuff_);

}


void InstrumentSettingsModel::selectSample(unsigned int index)
{
	GUIStuff_.nextSampleIndex_ = index;

	COUNTED_THREAD worker(workerSelectSample_, this, GUIStuff_);
	
}


void InstrumentSettingsModel::createNewSampleData()
{
	COUNTED_THREAD worker(workerCreateNewSampleData_, this, GUIStuff_);
	
}


void InstrumentSettingsModel::setRootNote(int rootNote)
{
	GUIStuff_.temporarySampleData_.setRootNote(rootNote);
}

void InstrumentSettingsModel::setLowNote(int lowNote)
{
	GUIStuff_.temporarySampleData_.setLowNote(lowNote);
}

void InstrumentSettingsModel::setHighNote(int highNote)
{
	GUIStuff_.temporarySampleData_.setHighNote(highNote);
}

void InstrumentSettingsModel::setLoopType(int type)
{
	GUIStuff_.temporarySampleData_.setLoopType(type);
}


int InstrumentSettingsModel::getRootNote()
{
	return GUIStuff_.temporarySampleData_.getRootNote();
}

int InstrumentSettingsModel::getLowNote()
{
	return GUIStuff_.temporarySampleData_.getLowNote();
}

int InstrumentSettingsModel::getHighNote()
{
	return GUIStuff_.temporarySampleData_.getHighNote();
}


std::string InstrumentSettingsModel::getNewInstrumentName()
{
	return GUIStuff_.newInstrumentName_;
}

INSTRUMENT_TYPE InstrumentSettingsModel::getNewInstrumentType()
{
	return GUIStuff_.newInstrumentType_;
}

unsigned int InstrumentSettingsModel::getSelectedInstrumentIndex()
{
	return GUIStuff_.currentInstrumentIndex_;
}

bool InstrumentSettingsModel::instrumentSelected()
{
	return GUIStuff_.instrumentSelected_;
}

unsigned int InstrumentSettingsModel::getSelectedSampleDataIndex()
{
	return GUIStuff_.currentSampleDataIndex_;
}

bool InstrumentSettingsModel::sampleDataSelected()
{
	return GUIStuff_.sampleDataSelected_;
}

unsigned int InstrumentSettingsModel::getSelectedSampleIndex()
{
	return GUIStuff_.currentSampleIndex_;
}

bool InstrumentSettingsModel::sampleSelected()
{
	return GUIStuff_.sampleSelected_;
}


InstrumentSettingsModelGUIStuff *InstrumentSettingsModel::getGUIStuff()
{
	return &GUIStuff_;
}


void InstrumentSettingsModel::registerObserver(Observer *observer)
{
	subject_.registerObserver(observer);
}

void InstrumentSettingsModel::unregisterObserver(Observer *observer)
{
	subject_.unregisterObserver(observer);
}
