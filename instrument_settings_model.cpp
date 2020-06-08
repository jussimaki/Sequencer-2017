#include <iostream>
#include <sstream>
//#include <chrono>
//#include <cstdlib>     

#include "instrument_settings_model.h"
#include "instrument_sample_model.h"
#include "wave.h"

#include "lock_levels.h"
#include "lock_guard.h"



extern thread_local LockHierarchy localLockHierarchy;

// worker functions that react to GUI events

void InstrumentSettingsModel::workerCreateNewInstrument(InstrumentSettingsModelGUIStuff GUIStuff)
{
	

	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);


	Instrument *instrument = new Instrument(instrumentIDIssuer_, GUIStuff.newInstrumentName_, GUIStuff.newInstrumentType_);

	if(instrumentSampleGuard.error()) instrument->setName("ERROR!");

	instrumentSampleModel_->addInstrument(instrument);
		
}

void InstrumentSettingsModel::workerCreateNewInstrument_(InstrumentSettingsModel *model, InstrumentSettingsModelGUIStuff GUIStuff)
{
	model->workerCreateNewInstrument(GUIStuff);
}

void InstrumentSettingsModel::workerDeleteInstrument(InstrumentSettingsModelGUIStuff GUIStuff)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	unsigned int amount = instrumentSampleModel_->getInstrumentAmount();

	ID <Instrument> instrumentID = temporaryInstrument_.getID();

	for (unsigned int index = 0; index < amount; index++)
	{
		Instrument *instrument = instrumentSampleModel_->getInstrument(index);

		if (instrument->getID() == instrumentID)
		{
			instrumentSampleModel_->deleteInstrument(index);
			break;
		}
		
	}

}

void InstrumentSettingsModel::workerDeleteInstrument_(InstrumentSettingsModel *model, InstrumentSettingsModelGUIStuff GUIStuff)
{
	model->workerDeleteInstrument(GUIStuff);
}





void InstrumentSettingsModel::workerSelectInstrument(InstrumentSettingsModelGUIStuff GUIStuff)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);
	
	// replace the corresponding instrument (has the same ID as the temporaryInstrument)
	// in the instrumentSampleModel with the modified temporary instrument
	
	if (GUIStuff.instrumentSelected_)
	{
		// first save the GUIStuff data to the temporary instrument
		saveTemporaryInstrumentData(GUIStuff);

		// then replace the instrument in the instrumentSampleModel
		if (!temporaryInstrument_.getID().isEmpty())
		{

			instrumentSampleModel_->changeInstrument(temporaryInstrument_);

			/*
			Instrument *instrument = instrumentSampleModel_->getInstrument(temporaryInstrument_.getID());

			if (instrument) { *instrument = temporaryInstrument_; }
			*/
		}
	}
	
	
	
	// fetch the next temporary instrument from instrumentSampleModel based on the clicked list index
	bool success = false;

	if ((instrumentSampleModel_->getInstrumentAmount() > GUIStuff.nextInstrumentIndex_) && (instrumentSampleModel_->getInstrumentAmount() > 0))
	{
		temporaryInstrument_ = *instrumentSampleModel_->getInstrument(GUIStuff.nextInstrumentIndex_);
		success = true;
	}
	
	// when selecting another instrument the sampledata names must be updated
	if(success) getSampleDataNames();

	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->INSTRUMENT_SELECTED = true;
	message->index = GUIStuff.nextInstrumentIndex_;
	message->SUCCESS = success;
	
	if(success) message->fetched_ = temporaryInstrument_;


	message->SAMPLEDATA_DESELECTED = true;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}

void InstrumentSettingsModel::workerSelectInstrument_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff)
{
	instrumentSettingsModel->workerSelectInstrument(GUIStuff);
}

void InstrumentSettingsModel::workerSelectSampleData(InstrumentSettingsModelGUIStuff GUIStuff)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	// save the GUIStuff data to the temporary instrument
	saveTemporaryInstrumentData(GUIStuff);

	


	bool success = false;

	if ((temporaryInstrument_.getSampleDataAmount() > 0) && (temporaryInstrument_.getSampleDataAmount() > GUIStuff.nextSampleDataIndex_))
		success = true;

	if (success)
	{
		for (unsigned int index = 0; index < instrumentSampleModel_->getSampleAmount(); index++)
		{
			Sample *sample = instrumentSampleModel_->getSample(index);
			if (sample->getID() == temporaryInstrument_.getSampleData(GUIStuff.nextSampleDataIndex_)->getSampleID())
				sampleDataSample_ = *sample;
				//if(sampleDataSample_.getID().getValue() != sample->getID().getValue())
					
		}
	}

		
	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->SAMPLEDATA_SELECTED = true;
	message->index = GUIStuff.nextSampleDataIndex_;
	message->SUCCESS = success;
	message->fetched_ = temporaryInstrument_;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
	
}



void InstrumentSettingsModel::workerSelectSampleData_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff)
{
	instrumentSettingsModel->workerSelectSampleData(GUIStuff);
}

void InstrumentSettingsModel::workerSelectSample(InstrumentSettingsModelGUIStuff GUIStuff)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	bool success = (instrumentSampleModel_->getSampleAmount() > GUIStuff.nextSampleIndex_);
	
			
	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->SAMPLE_SELECTED = true;
	message->index = GUIStuff.nextSampleIndex_;
	message->SUCCESS = success;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}



void InstrumentSettingsModel::workerSelectSample_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff)
{
	instrumentSettingsModel->workerSelectSample(GUIStuff);
}

void InstrumentSettingsModel::workerCreateNewSampleData(InstrumentSettingsModelGUIStuff GUIStuff)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	Sample *sample = 0;

	if ((instrumentSampleModel_->getSampleAmount() > 0) && (instrumentSampleModel_->getSampleAmount() > GUIStuff.currentSampleIndex_))
	{
		sample = instrumentSampleModel_->getSample(GUIStuff.currentSampleIndex_);

		SampleData sampleData(sampleDataIDIssuer_, sample);
		temporaryInstrument_.addSampleData(sampleData);

		instrumentSampleModel_->changeInstrument(temporaryInstrument_);
	}
}

void InstrumentSettingsModel::workerCreateNewSampleData_(InstrumentSettingsModel *model, InstrumentSettingsModelGUIStuff GUIStuff)
{
	model->workerCreateNewSampleData(GUIStuff);
}


// worker functions that respond to changes in instrumentSampleModel_

void InstrumentSettingsModel::workerInstrumentAdded()
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	getInstrumentNames();


	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->INSTRUMENT_NAMES_READ = true;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}

void InstrumentSettingsModel::workerInstrumentAdded_(InstrumentSettingsModel *instrumentSettingsModel)
{
	instrumentSettingsModel->workerInstrumentAdded();
}


void InstrumentSettingsModel::workerInstrumentChanged(ID <Instrument> ID)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	getInstrumentNames();
	getSampleNames();
	getSampleDataNames();

	if (!temporaryInstrument_.getID().isEmpty())
	{
		if (ID == temporaryInstrument_.getID())
		{
			getTemporaryInstrumentByID(ID);
		}
	}

	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->INSTRUMENT_CHANGED = true;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}

void InstrumentSettingsModel::workerInstrumentChanged_(InstrumentSettingsModel *instrumentSettingsModel, ID <Instrument> ID)
{
	instrumentSettingsModel->workerInstrumentChanged(ID);
}

void InstrumentSettingsModel::workerInstrumentDeleted(ID <Instrument> ID)
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	getInstrumentNames();

	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->INSTRUMENT_NAMES_READ = true;

	if (temporaryInstrument_.getID() == ID)
	{
		temporaryInstrument_.returnID();
		message->SELECTED_INSTRUMENT_DELETED = true;
	}

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);

}

void InstrumentSettingsModel::workerInstrumentDeleted_(InstrumentSettingsModel *instrumentSettingsModel, ID <Instrument> ID)
{
	instrumentSettingsModel->workerInstrumentDeleted(ID);
}

void InstrumentSettingsModel::workerSampleAdded()
{
	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	getSampleNames();

	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->SAMPLE_NAMES_READ = true;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}


void InstrumentSettingsModel::workerSampleAdded_(InstrumentSettingsModel *instrumentSettingsModel)
{
	instrumentSettingsModel->workerSampleAdded();
}

void InstrumentSettingsModel::workerSampleDeleted(InstrumentSettingsModelGUIStuff GUIStuff, ID <Sample> ID)
{
	

	LockGuard settingsGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	getSampleNames();
	
	InstrumentSettingsModelInternalMessage *message = new InstrumentSettingsModelInternalMessage(instrumentIDIssuer_, sampleDataIDIssuer_, sampleIDIssuer_);
	message->SAMPLE_NAMES_READ = true;
	
	if ((instrumentSampleModel_->getSampleAmount() > 0) && (instrumentSampleModel_->getSampleAmount() > GUIStuff.currentSampleIndex_))
	{
		if (instrumentSampleModel_->getSample(GUIStuff.currentSampleIndex_)->getID() == ID)
			message->SELECTED_SAMPLE_DELETED = true;
	}
	
			
	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	instrumentSettingsModelInternalRegisterer_->notify(message);
}

void InstrumentSettingsModel::workerSampleDeleted_(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsModelGUIStuff GUIStuff , ID <Sample> ID)
{
	instrumentSettingsModel->workerSampleDeleted(GUIStuff, ID);
}


void InstrumentSettingsModel::workerLoadSample(std::string fileName)
{
	
	//std::this_thread::sleep_until(std::chrono_literals::s);

	//Sleep(1000);
	//throw (std::runtime_error("ERROR!"));


	bool success = false;

	Wave wave;

	{
		LockGuard fileGuard(&localLockHierarchy, &fileLock_, 1, LOCK_LEVELS::IO);
		success = wave.load((char*)fileName.c_str());
	}

	if (success)
	{
		Sample *sample = new Sample(sampleIDIssuer_, fileName);
		sample->setData(wave.getBuffer());
		sample->setLength(wave.getSamplesPerChannel());
		sample->setSampleRate(wave.getSampleRate());

		wave.unLoad();
		
		LockGuard instrumentSampleGuard(&localLockHierarchy, &instrumentSampleLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);
		instrumentSampleModel_->addSample(sample);

	}

}

void InstrumentSettingsModel::workerLoadSample_(InstrumentSettingsModel *instrumentSettingsModel, std::string fileName)
{
	instrumentSettingsModel->workerLoadSample(fileName);
}




// helper functions

// NOTE: instrumentSettingsModelLock_ must be acquired!
void InstrumentSettingsModel::saveTemporaryInstrumentData(InstrumentSettingsModelGUIStuff GUIStuff)
{
	// saves the GUIStuff data to temporaryInstrument_
	
	if (GUIStuff.sampleDataSelected_)
	{
		if (!GUIStuff.temporarySampleData_.getSampleDataID().isEmpty())
		{
			temporaryInstrument_.addSampleData(GUIStuff.temporarySampleData_);
		}
	}

	temporaryInstrument_.setName(GUIStuff.temporaryInstrumentName_);
	temporaryInstrument_.setType(GUIStuff.temporaryInstrumentType_);
}



// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_ must be acquired!
void InstrumentSettingsModel::getTemporaryInstrumentByID(ID <Instrument> ID)
{
	unsigned int amount = instrumentSampleModel_->getInstrumentAmount();
	bool found = false;

	for (unsigned int index = 0; index < amount; index++)
	{
		Instrument *instrument = instrumentSampleModel_->getInstrument(index);

		if (instrument->getID() == ID)
		{
			temporaryInstrument_ = *instrument;
			found = true;

			break;
		}
	}
}

// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_ must be acquired!
bool InstrumentSettingsModel::getTemporaryInstrumentByIndex(unsigned int index)
{
	if (instrumentSampleModel_->getInstrumentAmount() > index && (instrumentSampleModel_->getInstrumentAmount() > 0))
	{
		
		temporaryInstrument_ = *instrumentSampleModel_->getInstrument(index);
		return true;
	}

	return false;
}



// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_ must be acquired!
void InstrumentSettingsModel::getInstrumentNames()
{
	instrumentNameList_.clear();

	for (unsigned int index = 0; index < instrumentSampleModel_->getInstrumentAmount(); index++)
	{
		Instrument *instrument = instrumentSampleModel_->getInstrument(index);

		std::string typeString;
		INSTRUMENT_TYPE type = instrument->getType();

		if (type == INSTRUMENT_MELODY) typeString = "Melody";
		else typeString = "Drum";

		std::stringstream idString;
		idString << instrument->getID().getValue();

		std::string nameAndType = instrument->getName() + " (" + typeString + ") " + idString.str();
		instrumentNameList_.push_back(nameAndType);
	}
}

// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_ must be acquired!
void InstrumentSettingsModel::getSampleNames()
{
	sampleNameList_.clear();

	for (unsigned int index = 0; index < instrumentSampleModel_->getSampleAmount(); index++)
	{
		sampleNameList_.push_back(instrumentSampleModel_->getSample(index)->getName());
	}


	{
		LockGuard coutGuard(&localLockHierarchy, &coutLock_, 1, LOCK_LEVELS::IO);

		std::cout << "worker locked coutlock at " << coutLock_ << std::endl;
		std::cout << "worker local lock hierarchy " << &localLockHierarchy << std::endl;
		std::cout << "sample names" << std::endl;

		for (unsigned int index = 0; index < sampleNameList_.size(); index++)
		{
			std::cout << sampleNameList_[index] << std::endl;
		}
	}
}

// NOTE: instrumentSettingsModelLock_ and instrumentSampleLock_ must be acquired!
void InstrumentSettingsModel::getSampleDataNames()
{
	sampleDataNameList_.clear();

	unsigned int amount = temporaryInstrument_.getSampleDataAmount();



	// go through the instrument's sample data
	for (unsigned int index = 0; index < amount; index++)
	{
		ID <Sample> ID = temporaryInstrument_.getSampleData(index)->getSampleID();


		// find if there's a sample in the instrumentSampleModel that corresponds the sampleData's sample ID
		for (unsigned int index2 = 0; index2 < instrumentSampleModel_->getSampleAmount(); index2++)
		{
			Sample *sample = instrumentSampleModel_->getSample(index2);

			if (sample->getID() == ID)
			{
				sampleDataNameList_.push_back(sample->getName());
			}
		}
	}
}



// NOTE: instrumentSettingsModelLock_ must be acquired!
std::vector <std::string> InstrumentSettingsModel::getInstrumentNameList()
{
	return instrumentNameList_;
}

// NOTE: instrumentSettingsModelLock_ must be acquired!
std::vector <std::string> InstrumentSettingsModel::getSampleNameList()
{
	return sampleNameList_;
}

// NOTE: instrumentSettingsModelLock_ must be acquired!
std::vector <std::string> InstrumentSettingsModel::getSampleDataNameList()
{
	return sampleDataNameList_;
}

// NOTE: instrumentSettingsModelLock_ must be acquired!
Instrument InstrumentSettingsModel::getTemporaryInstrument()
{
	return temporaryInstrument_;
}

// NOTE: instrumentSettingsModelLock_ must be acquired!
Sample InstrumentSettingsModel::getSampleDataSample()
{
	return sampleDataSample_;
}