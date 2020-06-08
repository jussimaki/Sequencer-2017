#include "instrument_settings_controller.h"

#include "instrument_settings_model.h"
#include "instrument_settings_view.h"

#include "file_window_view.h"
#include "number_to_string.h"


InstrumentSettingsController::InstrumentSettingsController(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsView *instrumentSettingsView) :
instrumentSettingsModel_(instrumentSettingsModel), instrumentSettingsView_(instrumentSettingsView)

//,
//loadSampleFileWindowController_(instrumentSettingsModel_->getLoadSampleFileWindowModel(), instrumentSettingsView_->getLoadSampleFileWindowView())
{
	setInstrumentSettingsController();
}

InstrumentSettingsController::~InstrumentSettingsController()
{
}

void InstrumentSettingsController::setInstrumentSettingsController()
{
	// Instrument settings window
	instrumentSettingsView_->setInstrumentListCallback(this, InstrumentSettingsController::instrumentListClicked_);
	instrumentSettingsView_->setSampleMappingButtonCallback(this, InstrumentSettingsController::sampleMappingButtonClicked_);
	instrumentSettingsView_->setNewInstrumentButtonCallback(this, InstrumentSettingsController::newInstrumentButtonClicked_);
	instrumentSettingsView_->setDeleteInstrumentButtonCallback(this, InstrumentSettingsController::deleteInstrumentButtonClicked_);
	instrumentSettingsView_->setCloseButtonCallback(this, InstrumentSettingsController::closeButtonClicked_);

	// New instrument window
	instrumentSettingsView_->setNewInstrumentNameCallback(this, InstrumentSettingsController::newInstrumentNameChanged_);
	instrumentSettingsView_->setNewInstrumentTypeCallback(this, InstrumentSettingsController::newInstrumentTypeChanged_);
	instrumentSettingsView_->setNewInstrumentOkButtonCallback(this, InstrumentSettingsController::newInstrumentOkButtonClicked_);
	instrumentSettingsView_->setNewInstrumentCancelButtonCallback(this, InstrumentSettingsController::newInstrumentCancelButtonClicked_);

	// Delete instrument window
	instrumentSettingsView_->setDeleteInstrumentOkButtonCallback(this, InstrumentSettingsController::deleteInstrumentOkButtonClicked_);
	instrumentSettingsView_->setDeleteInstrumentCancelButtonCallback(this, InstrumentSettingsController::deleteInstrumentCancelButtonClicked_);

	// Sample mapping window

	instrumentSettingsView_->setAddSampleButtonCallback(this, InstrumentSettingsController::addSampleButtonClicked_);
	instrumentSettingsView_->setRemoveSampleButtonCallback(this, InstrumentSettingsController::removeSampleButtonClicked_);
	instrumentSettingsView_->setSampleMappingCloseButtonCallback(this, InstrumentSettingsController::sampleMappingCloseButtonClicked_);
	instrumentSettingsView_->setSampleListCallback(this, InstrumentSettingsController::sampleListCallback_);
	instrumentSettingsView_->setRootNoteCallback(this, InstrumentSettingsController::rootNoteCallback_);
	instrumentSettingsView_->setLowNoteCallback(this, InstrumentSettingsController::lowNoteCallback_);
	instrumentSettingsView_->setHighNoteCallback(this, InstrumentSettingsController::highNoteCallback_);
	instrumentSettingsView_->setLoopTypeCallback(this, InstrumentSettingsController::loopTypeCallback_);
	instrumentSettingsView_->setSampleAttackCallback(this, InstrumentSettingsController::attackCallback_);
	instrumentSettingsView_->setSampleDecayCallback(this, InstrumentSettingsController::decayCallback_);
	instrumentSettingsView_->setSampleSustainCallback(this, InstrumentSettingsController::sustainCallback_);
	instrumentSettingsView_->setSampleReleaseCallback(this, InstrumentSettingsController::releaseCallback_);
	instrumentSettingsView_->setPlaySampleButtonCallback(this, InstrumentSettingsController::playSampleButtonClicked_);
	instrumentSettingsView_->setSampleZoomInButtonCallback(this, InstrumentSettingsController::zoomInButtonClicked_);
	instrumentSettingsView_->setSampleZoomOutButtonCallback(this, InstrumentSettingsController::zoomOutButtonClicked_);
	instrumentSettingsView_->setLoopStartCallback(this, InstrumentSettingsController::loopStartCallback_);
	instrumentSettingsView_->setLoopEndCallback(this, InstrumentSettingsController::loopEndCallback_);

	// Add sample window
	instrumentSettingsView_->setAddSampleOkButtonCallback(this, InstrumentSettingsController::addSampleOkButtonClicked_);
	instrumentSettingsView_->setAddSampleCancelButtonCallback(this, InstrumentSettingsController::addSampleCancelButtonClicked_);
	instrumentSettingsView_->setLoadSampleButtonCallback(this, InstrumentSettingsController::loadSampleButtonClicked_);
	instrumentSettingsView_->setLoadedSamplesListCallback(this, InstrumentSettingsController::loadedSamplesListClicked_);
}

// Instrument settings window


void InstrumentSettingsController::instrumentListClicked(ChooseEvent *event)
{
	instrumentSettingsModel_->selectInstrument(event->getNumber());
}

void InstrumentSettingsController::instrumentListClicked_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->instrumentListClicked(event);
}

void InstrumentSettingsController::sampleMappingButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->openSampleMappingWindow();
}

void InstrumentSettingsController::sampleMappingButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->sampleMappingButtonClicked(event);
}

void InstrumentSettingsController::newInstrumentButtonClicked(MouseEvent *event)
{
	instrumentSettingsModel_->setNewInstrumentName("");
	instrumentSettingsModel_->setNewInstrumentType(INSTRUMENT_MELODY);

	instrumentSettingsView_->openNewInstrumentWindow();
	instrumentSettingsView_->updateNewInstrumentWindow();
	
}

void InstrumentSettingsController::newInstrumentButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->newInstrumentButtonClicked(event);
}

void InstrumentSettingsController::deleteInstrumentButtonClicked(MouseEvent *event)
{
	if (instrumentSettingsModel_->instrumentSelected())
	{
		instrumentSettingsView_->openDeleteInstrumentWindow();
	}
}

void InstrumentSettingsController::deleteInstrumentButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->deleteInstrumentButtonClicked(event);
}

void InstrumentSettingsController::closeButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeInstrumentSettingsView();
}

void InstrumentSettingsController::closeButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->closeButtonClicked(event);
}


// New instrument window

void InstrumentSettingsController::newInstrumentNameChanged(TextEvent *event)
{
	instrumentSettingsModel_->setNewInstrumentName(event->getText());
}

void InstrumentSettingsController::newInstrumentNameChanged_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->newInstrumentNameChanged(event);
}

void InstrumentSettingsController::newInstrumentTypeChanged(ChooseEvent *event)
{
	instrumentSettingsModel_->setNewInstrumentType((INSTRUMENT_TYPE)event->getNumber());
}

void InstrumentSettingsController::newInstrumentTypeChanged_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->newInstrumentTypeChanged(event);
}

void InstrumentSettingsController::newInstrumentOkButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeNewInstrumentWindow();

	instrumentSettingsModel_->createNewInstrument();
}

void InstrumentSettingsController::newInstrumentOkButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->newInstrumentOkButtonClicked(event);
}

void InstrumentSettingsController::newInstrumentCancelButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeNewInstrumentWindow();
}

void InstrumentSettingsController::newInstrumentCancelButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->newInstrumentCancelButtonClicked(event);
}


// Delete instrument window


void InstrumentSettingsController::deleteInstrumentOkButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeDeleteInstrumentWindow();
	
	instrumentSettingsModel_->deleteInstrument();
	
}

void InstrumentSettingsController::deleteInstrumentOkButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->deleteInstrumentOkButtonClicked(event);
}


void InstrumentSettingsController::deleteInstrumentCancelButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeDeleteInstrumentWindow();
}

void InstrumentSettingsController::deleteInstrumentCancelButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->deleteInstrumentCancelButtonClicked(event);
}


// Sample mapping window

void InstrumentSettingsController::addSampleButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->openAddSampleWindow();
}

void InstrumentSettingsController::addSampleButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->addSampleButtonClicked(event);
}

void InstrumentSettingsController::removeSampleButtonClicked(MouseEvent *event)
{
}

void InstrumentSettingsController::removeSampleButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->removeSampleButtonClicked(event);
}

void InstrumentSettingsController::sampleMappingCloseButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeSampleMappingWindow();
}

void InstrumentSettingsController::sampleMappingCloseButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->sampleMappingCloseButtonClicked(event);
}

void InstrumentSettingsController::sampleListCallback(ChooseEvent *event)
{
	instrumentSettingsModel_->selectSampleData(event->getNumber());
}

void InstrumentSettingsController::sampleListCallback_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->sampleListCallback(event);
}

void InstrumentSettingsController::rootNoteCallback(ChooseEvent *event)
{
	instrumentSettingsModel_->setRootNote(event->getNumber());
}

void InstrumentSettingsController::rootNoteCallback_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->rootNoteCallback(event);
}

void InstrumentSettingsController::lowNoteCallback(ChooseEvent *event)
{
	instrumentSettingsModel_->setLowNote(event->getNumber());
}

void InstrumentSettingsController::lowNoteCallback_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->lowNoteCallback(event);
}

void InstrumentSettingsController::highNoteCallback(ChooseEvent *event)
{
	instrumentSettingsModel_->setHighNote(event->getNumber());
}

void InstrumentSettingsController::highNoteCallback_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->highNoteCallback(event);
}

void InstrumentSettingsController::loopTypeCallback(ChooseEvent *event)
{
	instrumentSettingsModel_->setLoopType(event->getNumber());
}

void InstrumentSettingsController::loopTypeCallback_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->loopTypeCallback(event);
}

void InstrumentSettingsController::attackCallback(TextEvent *event)
{
	double number;
	bool value = stringToDouble(event->getText(), &number);

	if(value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setAttack(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::attackCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->attackCallback(event);
}

void InstrumentSettingsController::decayCallback(TextEvent *event)
{
	double number;
	bool value = stringToDouble(event->getText(), &number);

	if (value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setDecay(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::decayCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->decayCallback(event);
}

void InstrumentSettingsController::sustainCallback(TextEvent *event)
{
	double number;
	bool value = stringToDouble(event->getText(), &number);

	if (value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setSustain(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::sustainCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->sustainCallback(event);
}

void InstrumentSettingsController::releaseCallback(TextEvent *event)
{
	double number;
	bool value = stringToDouble(event->getText(), &number);

	if (value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setRelease(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::releaseCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->releaseCallback(event);
}

void InstrumentSettingsController::playSampleButtonClicked(MouseEvent *event){}

void InstrumentSettingsController::playSampleButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->playSampleButtonClicked(event);
}

void InstrumentSettingsController::zoomInButtonClicked(MouseEvent *event){}

void InstrumentSettingsController::zoomInButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->zoomInButtonClicked(event);
}

void InstrumentSettingsController::zoomOutButtonClicked(MouseEvent *event){}

void InstrumentSettingsController::zoomOutButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->zoomOutButtonClicked(event);
}

void InstrumentSettingsController::loopStartCallback(TextEvent *event)
{
	int number;
	bool value = stringToInt(event->getText(), &number);

	if (value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setLoopStart(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::loopStartCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->loopStartCallback(event);
}

void InstrumentSettingsController::loopEndCallback(TextEvent *event)
{
	int number;
	bool value = stringToInt(event->getText(), &number);

	if (value) instrumentSettingsModel_->getGUIStuff()->temporarySampleData_.setLoopEnd(number);

	instrumentSettingsView_->update(0);
}

void InstrumentSettingsController::loopEndCallback_(void *object, TextEvent *event)
{
	((InstrumentSettingsController*)object)->loopEndCallback(event);
}


// Add sample window

void InstrumentSettingsController::addSampleOkButtonClicked(MouseEvent *event)
{
	instrumentSettingsModel_->createNewSampleData();

	instrumentSettingsView_->closeAddSampleWindow();
}

void InstrumentSettingsController::addSampleOkButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->addSampleOkButtonClicked(event);
}

void InstrumentSettingsController::addSampleCancelButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->closeAddSampleWindow();
}

void InstrumentSettingsController::addSampleCancelButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->addSampleCancelButtonClicked(event);
}

void InstrumentSettingsController::loadSampleButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->openLoadSampleDialog();
	//loadSampleFileWindowController_.openFileWindow();

}

void InstrumentSettingsController::loadSampleButtonClicked_(void *object, MouseEvent *event)
{
	((InstrumentSettingsController*)object)->loadSampleButtonClicked(event);
}

void InstrumentSettingsController::loadedSamplesListClicked(ChooseEvent *event)
{
	instrumentSettingsModel_->selectSample(event->getNumber());
}

void InstrumentSettingsController::loadedSamplesListClicked_(void *object, ChooseEvent *event)
{
	((InstrumentSettingsController*)object)->loadedSamplesListClicked(event);
}