#include <sstream>

#include "spawn_thread.h"
#include "instrument_settings_view.h"
#include "instrument_settings_model.h"

InstrumentSettingsView::InstrumentSettingsView(CASLock *instrumentSettingsModelLock, CASLock *instrumentSettingsViewLock, CASLock *eventLock, CASLock *coutLock,
	CallbackConcreteRegisterer<InstrumentSettingsViewInternalMessage> *instrumentSettingsViewInternalRegisterer,
	IDIssuer <Instrument> *instrumentIDIssuer, IDIssuer <Sample> *sampleIDIssuer,
	InstrumentSettingsModel *instrumentSettingsModel, WindowSystem *windowSystem,
	FileWindowView *loadSampleFileWindowView) :
	instrumentSettingsModelLock_(instrumentSettingsModelLock),
	instrumentSettingsViewLock_(instrumentSettingsViewLock),
	eventLock_(eventLock),
	coutLock_(coutLock),
	instrumentSettingsViewInternalRegisterer_(instrumentSettingsViewInternalRegisterer),
	instrumentSettingsModel_(instrumentSettingsModel),
	windowSystem_(windowSystem),
	sampleDataSample_(sampleIDIssuer, ""),
	sampleCanvasUpdated_(false),
	deleteInstrumentDialog_(windowSystem, "", "Ok", "Cancel"),
	loadSampleFileWindowView_(loadSampleFileWindowView)
{
	instrumentSettingsViewInternalRegisterer_->registerListener(this, &internalCallback_);

	instrumentSettingsModel_->registerObserver(this);


	setInstrumentSettingsView();
	setNewInstrumentWindow();
	setSampleMappingWindow();
	setAddSampleWindow();

	//update(0);
}


InstrumentSettingsView::~InstrumentSettingsView()
{
	instrumentSettingsModel_->unregisterObserver(this);
	instrumentSettingsViewInternalRegisterer_->unregisterListener(this);
}



// Instrument settings window

void InstrumentSettingsView::setInstrumentSettingsView()
{
	instrumentSettingsWindow_.addRegion("InstrumentLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		400, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	instrumentSettingsWindow_.addRegion("InstrumentList",
		0, ATTACH_LEFT, "InstrumentLabel",
		10, ATTACH_BOTTOM, "InstrumentLabel",
		250, ATTACH_NONE, "",
		400, ATTACH_NONE, "");

	instrumentSettingsWindow_.addRegion("SampleMappingButton",
		10, ATTACH_RIGHT, "InstrumentList",
		0, ATTACH_TOP, "InstrumentList",
		150, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentSettingsWindow_.addRegion("NewInstrumentButton",
		0, ATTACH_LEFT, "InstrumentList",
		10, ATTACH_BOTTOM, "InstrumentList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentSettingsWindow_.addRegion("DeleteInstrumentButton",
		10, ATTACH_RIGHT, "NewInstrumentButton",
		0, ATTACH_TOP, "NewInstrumentButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentSettingsWindow_.addRegion("CloseButton",
		0, ATTACH_LEFT, "NewInstrumentButton",
		10, ATTACH_BOTTOM, "NewInstrumentButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");


	instrumentSettingsWindow_.setWidget("InstrumentLabel", &instrumentLabel_);
	instrumentSettingsWindow_.setWidget("NewInstrumentButton", &newInstrumentButton_);
	instrumentSettingsWindow_.setWidget("DeleteInstrumentButton", &deleteInstrumentButton_);
	instrumentSettingsWindow_.setWidget("InstrumentList", &instrumentList_);
	instrumentSettingsWindow_.setWidget("SampleMappingButton", &sampleMappingButton_);
	instrumentSettingsWindow_.setWidget("CloseButton", &closeButton_);

	instrumentSettingsWindow_.setVScrollControlsActive(true);


	instrumentLabel_.text_ = "Instruments:";


	newInstrumentButton_.setName("New");
	deleteInstrumentButton_.setName("Delete");
	sampleMappingButton_.setName("Sample mapping");
	closeButton_.setName("Close");

	instrumentList_.setVScrollControlsActive(true);
	instrumentList_.setHScrollControlsActive(true);
}

void InstrumentSettingsView::openInstrumentSettingsView()
{
	int x, y;

	windowSystem_->getTopWindowCoordinates(&x, &y);

	instrumentSettingsWindow_.setX(x + 20);
	instrumentSettingsWindow_.setY(y + 20);

	instrumentSettingsWindow_.setWidth(500);
	instrumentSettingsWindow_.setHeight(500);

	windowSystem_->addWindow(&instrumentSettingsWindow_);
}

void InstrumentSettingsView::closeInstrumentSettingsView()
{
	windowSystem_->closeWindow(&instrumentSettingsWindow_);
}

bool InstrumentSettingsView::updateInstrumentSettingsView()
{
	
	if (instrumentSettingsModel_->instrumentSelected())
	{
		instrumentList_.setSelected(true);

		instrumentList_.setRow(instrumentSettingsModel_->getSelectedInstrumentIndex());

		deleteInstrumentButton_.setDisabled(false);
		sampleMappingButton_.setDisabled(false);
	}
	else
	{
		instrumentList_.setSelected(false);
		deleteInstrumentButton_.setDisabled(true);
		sampleMappingButton_.setDisabled(true);
	}

	return true;
}


void InstrumentSettingsView::setInstrumentListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	instrumentList_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleMappingButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	sampleMappingButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setNewInstrumentButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	newInstrumentButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setDeleteInstrumentButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	deleteInstrumentButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setCloseButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	closeButton_.onClick_.add(object, functionPointer);
}

// New instrument window

void InstrumentSettingsView::setNewInstrumentWindow()
{
	newInstrumentWindow_.addRegion("NewInstrumentLabel",
		10, ATTACH_NONE, "",
		10, ATTACH_NONE, "",
		200, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentNameLabel",
		0, ATTACH_LEFT, "NewInstrumentLabel",
		10, ATTACH_BOTTOM, "NewInstrumentLabel",
		150, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentNameTextField",
		10, ATTACH_RIGHT, "NewInstrumentNameLabel",
		0, ATTACH_TOP, "NewInstrumentNameLabel",
		200, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentTypeLabel",
		0, ATTACH_LEFT, "NewInstrumentNameLabel",
		10, ATTACH_BOTTOM, "NewInstrumentNameLabel",
		150, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentTypeRadioButtons",
		10, ATTACH_RIGHT, "NewInstrumentTypeLabel",
		0, ATTACH_TOP, "NewInstrumentTypeLabel",
		200, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentOkButton",
		0, ATTACH_LEFT, "NewInstrumentTypeLabel",
		10, ATTACH_BOTTOM, "NewInstrumentTypeLabel",
		100, ATTACH_NONE, "",
		40, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentCancelButton",
		10, ATTACH_RIGHT, "NewInstrumentOkButton",
		0, ATTACH_TOP, "NewInstrumentOkButton",
		100, ATTACH_NONE, "",
		40, ATTACH_NONE, "");



	newInstrumentWindow_.setWidget("NewInstrumentLabel", &newInstrumentLabel_);
	newInstrumentWindow_.setWidget("NewInstrumentNameLabel", &newInstrumentNameLabel_);
	newInstrumentWindow_.setWidget("NewInstrumentNameTextField", &newInstrumentNameTextField_);

	newInstrumentWindow_.setWidget("NewInstrumentTypeLabel", &newInstrumentTypeLabel_);
	newInstrumentWindow_.setWidget("NewInstrumentTypeRadioButtons", &newInstrumentTypeRadioButtons_);

	newInstrumentWindow_.setWidget("NewInstrumentOkButton", &newInstrumentOkButton_);
	newInstrumentWindow_.setWidget("NewInstrumentCancelButton", &newInstrumentCancelButton_);

	newInstrumentLabel_.text_ = "Create a new instrument?";
	newInstrumentNameLabel_.text_ = "Instrument name:";

	newInstrumentTypeLabel_.text_ = "Instrument type:";

	newInstrumentTypeRadioButtons_.add("Melody");
	newInstrumentTypeRadioButtons_.add("Drums");

	newInstrumentOkButton_.setName("Ok");
	newInstrumentCancelButton_.setName("Cancel");
}

void InstrumentSettingsView::openNewInstrumentWindow()
{
	int x, y;

	windowSystem_->getTopWindowCoordinates(&x, &y);

	newInstrumentWindow_.setX(x + 20);
	newInstrumentWindow_.setY(y + 20);

	newInstrumentWindow_.setWidth(500);
	newInstrumentWindow_.setHeight(500);

	windowSystem_->addWindow(&newInstrumentWindow_);
}

void InstrumentSettingsView::closeNewInstrumentWindow()
{
	windowSystem_->closeWindow(&newInstrumentWindow_);
}

bool InstrumentSettingsView::updateNewInstrumentWindow()
{
	
	newInstrumentNameTextField_.setText(instrumentSettingsModel_->getNewInstrumentName());
	newInstrumentTypeRadioButtons_.setButtonNumber(instrumentSettingsModel_->getNewInstrumentType());

	return true;
}

void InstrumentSettingsView::setNewInstrumentNameCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	newInstrumentNameTextField_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setNewInstrumentTypeCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	newInstrumentTypeRadioButtons_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setNewInstrumentOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	newInstrumentOkButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setNewInstrumentCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	newInstrumentCancelButton_.onClick_.add(object, functionPointer);
}

// Delete instrument window

void InstrumentSettingsView::openDeleteInstrumentWindow()
{
	deleteInstrumentDialog_.openOkCancelDialog();
}

bool InstrumentSettingsView::updateDeleteInstrumentWindow()
{
	std::stringstream stream;

	stream << "Delete instrument " << instrumentSettingsModel_->getGUIStuff()->temporaryInstrumentName_ << " ?";
	deleteInstrumentDialog_.setMessage(stream.str());
	
	return true;
}


void InstrumentSettingsView::closeDeleteInstrumentWindow()
{
	deleteInstrumentDialog_.closeOkCancelDialog();
}

void InstrumentSettingsView::setDeleteInstrumentOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	deleteInstrumentDialog_.setOkButtonCallback(object, functionPointer);
}

void InstrumentSettingsView::setDeleteInstrumentCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	deleteInstrumentDialog_.setCancelButtonCallback(object, functionPointer);
}

void InstrumentSettingsView::setDeleteInstrumentMessage(std::string message)
{
	deleteInstrumentDialog_.setMessage(message);
}

// Sample mapping window


void InstrumentSettingsView::setSampleMappingWindow()
{

	sampleMappingWindow_.setWidth(1400);
	sampleMappingWindow_.setHeight(700);

	
	sampleMappingWindow_.addRegion("SampleMappingLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		400, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SamplesLabel",
		0, ATTACH_LEFT, "SampleMappingLabel",
		10, ATTACH_BOTTOM, "SampleMappingLabel",
		200, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleList",
		0, ATTACH_LEFT, "SamplesLabel",
		10, ATTACH_BOTTOM, "SamplesLabel",
		300, ATTACH_NONE, "",
		400, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("AddSampleButton",
		0, ATTACH_LEFT, "SampleList",
		10, ATTACH_BOTTOM, "SampleList",
		100, ATTACH_NONE, "",
		30, ATTACH_NONE, "");


	sampleMappingWindow_.addRegion("RemoveSampleButton",
		10, ATTACH_RIGHT, "AddSampleButton",
		0, ATTACH_TOP, "AddSampleButton",
		70, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleMappingCloseButton",
		0, ATTACH_LEFT, "AddSampleButton",
		10, ATTACH_BOTTOM, "AddSampleButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleMappingSampleName",
		10, ATTACH_RIGHT, "SampleList",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleLengthLabel",
		0, ATTACH_LEFT, "SampleMappingSampleName",
		5, ATTACH_BOTTOM, "SampleMappingSampleName",
		300, ATTACH_NONE, "",
		25, ATTACH_NONE, "");



	sampleMappingWindow_.addRegion("RootNoteLabel",
		0, ATTACH_LEFT, "SampleLengthLabel",
		10, ATTACH_BOTTOM, "SampleLengthLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("RootNoteMenu",
		10, ATTACH_RIGHT, "RootNoteLabel",
		0, ATTACH_TOP, "RootNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LowNoteLabel",
		0, ATTACH_LEFT, "RootNoteLabel",
		10, ATTACH_BOTTOM, "RootNoteLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LowNoteMenu",
		10, ATTACH_RIGHT, "LowNoteLabel",
		0, ATTACH_TOP, "LowNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("HighNoteLabel",
		0, ATTACH_LEFT, "LowNoteLabel",
		10, ATTACH_BOTTOM, "LowNoteLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("HighNoteMenu",
		10, ATTACH_RIGHT, "HighNoteLabel",
		0, ATTACH_TOP, "HighNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleAttackLabel",
		0, ATTACH_LEFT, "HighNoteLabel",
		10, ATTACH_BOTTOM, "HighNoteLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleAttack",
		10, ATTACH_RIGHT, "SampleAttackLabel",
		0, ATTACH_TOP, "SampleAttackLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleDecayLabel",
		0, ATTACH_LEFT, "SampleAttackLabel",
		10, ATTACH_BOTTOM, "SampleAttackLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleDecay",
		10, ATTACH_RIGHT, "SampleDecayLabel",
		0, ATTACH_TOP, "SampleDecayLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleSustainLabel",
		0, ATTACH_LEFT, "SampleDecayLabel",
		10, ATTACH_BOTTOM, "SampleDecayLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleSustain",
		10, ATTACH_RIGHT, "SampleSustainLabel",
		0, ATTACH_TOP, "SampleSustainLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleReleaseLabel",
		0, ATTACH_LEFT, "SampleSustainLabel",
		10, ATTACH_BOTTOM, "SampleSustainLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleRelease",
		10, ATTACH_RIGHT, "SampleReleaseLabel",
		0, ATTACH_TOP, "SampleReleaseLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("PlaySampleButton",
		0, ATTACH_LEFT, "SampleReleaseLabel",
		10, ATTACH_BOTTOM, "SampleReleaseLabel",
		100, ATTACH_NONE, "",
		50, ATTACH_NONE, "");





	sampleMappingWindow_.addRegion("SampleZoomInButton",
		60, ATTACH_RIGHT, "RootNoteMenu",
		0, ATTACH_TOP, "RootNoteMenu",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleZoomOutButton",
		10, ATTACH_RIGHT, "SampleZoomInButton",
		0, ATTACH_TOP, "SampleZoomInButton",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopStartLabel",
		10, ATTACH_RIGHT, "SampleZoomOutButton",
		0, ATTACH_TOP, "SampleZoomOutButton",
		70, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopStart",
		5, ATTACH_RIGHT, "LoopStartLabel",
		0, ATTACH_TOP, "LoopStartLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopEndLabel",
		10, ATTACH_RIGHT, "LoopStart",
		0, ATTACH_TOP, "LoopStart",
		70, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopEnd",
		5, ATTACH_RIGHT, "LoopEndLabel",
		0, ATTACH_TOP, "LoopEndLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopTypeLabel",
		0, ATTACH_LEFT, "SampleZoomInButton",
		10, ATTACH_BOTTOM, "SampleZoomInButton",
		40, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopType",
		5, ATTACH_RIGHT, "LoopTypeLabel",
		0, ATTACH_TOP, "LoopTypeLabel",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleGFXWindow",
		0, ATTACH_LEFT, "LoopTypeLabel",
		10, ATTACH_BOTTOM, "LoopTypeLabel",
		800, ATTACH_NONE, "",
		400, ATTACH_NONE, "");

	sampleMappingWindow_.setWidget("SampleMappingLabel", &sampleMappingLabel_);
	sampleMappingWindow_.setWidget("SamplesLabel", &samplesLabel_);
	sampleMappingWindow_.setWidget("SampleList", &sampleList_);

	sampleMappingWindow_.setWidget("AddSampleButton", &addSampleButton_);
	sampleMappingWindow_.setWidget("RemoveSampleButton", &removeSampleButton_);
	sampleMappingWindow_.setWidget("SampleMappingCloseButton", &sampleMappingCloseButton_);

	sampleMappingWindow_.setWidget("SampleMappingSampleName", &sampleMappingSampleName_);
	sampleMappingWindow_.setWidget("SampleLengthLabel", &sampleLength_);
	sampleMappingWindow_.setWidget("SampleZoomInButton", &sampleZoomInButton_);
	sampleMappingWindow_.setWidget("SampleZoomOutButton", &sampleZoomOutButton_);
	sampleMappingWindow_.setWidget("LoopStartLabel", &loopStartLabel_);
	sampleMappingWindow_.setWidget("LoopStart", &loopStart_);
	sampleMappingWindow_.setWidget("LoopEndLabel", &loopEndLabel_);
	sampleMappingWindow_.setWidget("LoopEnd", &loopEnd_);
	

	sampleMappingWindow_.setWidget("RootNoteLabel", &rootNoteLabel_);
	sampleMappingWindow_.setWidget("LowNoteLabel", &lowNoteLabel_);
	sampleMappingWindow_.setWidget("HighNoteLabel", &highNoteLabel_);
	sampleMappingWindow_.setWidget("RootNoteMenu", &rootNoteMenu_);
	sampleMappingWindow_.setWidget("LowNoteMenu", &lowNoteMenu_);
	sampleMappingWindow_.setWidget("HighNoteMenu", &highNoteMenu_);
	sampleMappingWindow_.setWidget("LoopTypeLabel", &loopTypeLabel_);
	sampleMappingWindow_.setWidget("LoopType", &loopType_);
	sampleMappingWindow_.setWidget("SampleAttackLabel", &sampleAttackLabel_);
	sampleMappingWindow_.setWidget("SampleAttack", &sampleAttack_);
	sampleMappingWindow_.setWidget("SampleDecayLabel", &sampleDecayLabel_);
	sampleMappingWindow_.setWidget("SampleDecay", &sampleDecay_);
	sampleMappingWindow_.setWidget("SampleSustainLabel", &sampleSustainLabel_);
	sampleMappingWindow_.setWidget("SampleSustain", &sampleSustain_);
	sampleMappingWindow_.setWidget("SampleReleaseLabel", &sampleReleaseLabel_);
	sampleMappingWindow_.setWidget("SampleRelease", &sampleRelease_);

	sampleMappingWindow_.setWidget("PlaySampleButton", &playSampleButton_);

	sampleMappingWindow_.setWidget("SampleGFXWindow", &sampleGFXWindow_);


	samplesLabel_.text_ = "Samples:";
	addSampleButton_.setName("Add sample");
	removeSampleButton_.setName("Remove");
	sampleMappingCloseButton_.setName("Close");

	rootNoteLabel_.text_ = "Root note:";
	lowNoteLabel_.text_ = "Low note:";
	highNoteLabel_.text_ = "High note:";

	loopTypeLabel_.text_ = "Loop:";

	loopType_.clear();
	loopType_.add("Off");
	loopType_.add("On");
	loopType_.add("Pingpong");



	sampleAttackLabel_.text_ = "Attack:";
	sampleDecayLabel_.text_ = "Decay:";
	sampleSustainLabel_.text_ = "Sustain:";
	sampleReleaseLabel_.text_ = "Release:";

	playSampleButton_.setName("Play");



	loopStartLabel_.text_ = "Loop start:";
	loopEndLabel_.text_ = "Loop end:";

	sampleList_.setHScrollControlsActive(true);
	sampleList_.setVScrollControlsActive(true);



	sampleZoomInButton_.setName("Z+");
	sampleZoomOutButton_.setName("Z-");


	sampleGFXWindow_.addDrawFunction(this, InstrumentSettingsView::drawWaveForm_);


	sampleGFXWindow_.setHScrollControlsActive(true);

	waveFormSprite_ = windowSystem_->getEmptyWindowSprite(600, 300);
	waveFormCanvas_ = windowSystem_->getWindowCanvas(waveFormSprite_);
	

	std::vector <std::string> *data1 = new std::vector <std::string>;
	std::vector <std::string> *data2 = new std::vector <std::string>;
	std::vector <std::string> *data3 = new std::vector <std::string>;

	for (int index = 0; index < 128; index++)
	{
		int note = index % 12;
		int octave = index / 12;

		std::string noteString;

		switch (note)
		{
		case 0:
			noteString = "c";
			break;
		case 1:
			noteString = "c#";
			break;
		case 2:
			noteString = "d";
			break;
		case 3:
			noteString = "d#";
			break;
		case 4:
			noteString = "e";
			break;
		case 5:
			noteString = "f";
			break;
		case 6:
			noteString = "f#";
			break;
		case 7:
			noteString = "g";
			break;
		case 8:
			noteString = "g#";
			break;
		case 9:
			noteString = "a";
			break;
		case 10:
			noteString = "a#";
			break;
		case 11:
			noteString = "b";
			break;
		default: break;
		}

		std::stringstream stream;
		stream << noteString;
		stream << "-";
		stream << octave;


		std::string string;
		stream >> string;


		data1->push_back(string);
		data2->push_back(string);
		data3->push_back(string);

		
	}



	rootNoteMenu_.setNextElements(data1);
	lowNoteMenu_.setNextElements(data2);
	highNoteMenu_.setNextElements(data3);

}

void InstrumentSettingsView::openSampleMappingWindow()
{
	int x, y;

	windowSystem_->getTopWindowCoordinates(&x, &y);
	sampleMappingWindow_.setX(x + 30);
	sampleMappingWindow_.setY(y + 30);

	windowSystem_->addWindow(&sampleMappingWindow_);
}

bool InstrumentSettingsView::updateSampleMappingWindow()
{
	bool success = localLockHierarchy.tryToAcquire(&instrumentSettingsViewLock_, 1, LOCK_LEVELS::GUI);

	if (!success) return false;



	InstrumentSettingsModelGUIStuff *GUIStuff = instrumentSettingsModel_->getGUIStuff();

	sampleMappingLabel_.text_ = "Instrument " + instrumentSettingsModel_->getTemporaryInstrument().getName() + " sample mapping:";
		
	if (instrumentSettingsModel_->instrumentSelected())
	{
		instrumentList_.setSelected(true);
		instrumentList_.setRow(instrumentSettingsModel_->getSelectedInstrumentIndex());

	}
	else instrumentList_.setSelected(false);

	if (instrumentSettingsModel_->sampleDataSelected())
	{
		sampleList_.setSelected(true);
		sampleList_.setRow(instrumentSettingsModel_->getSelectedSampleDataIndex());
		
		rootNoteMenu_.setDisabled(false);
		rootNoteMenu_.setRow(GUIStuff->temporarySampleData_.getRootNote());

		lowNoteMenu_.setDisabled(false);
		lowNoteMenu_.setRow(GUIStuff->temporarySampleData_.getLowNote());

		highNoteMenu_.setDisabled(false);
		highNoteMenu_.setRow(GUIStuff->temporarySampleData_.getHighNote());

		loopType_.setDisabled(false);
		loopType_.setButtonNumber(GUIStuff->temporarySampleData_.getLoopType());
		
		std::stringstream stream;

		sampleAttack_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getAttack();
		sampleAttack_.setText(stream.str());

		sampleDecay_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getDecay();
		sampleDecay_.setText(stream.str());

		sampleSustain_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getSustain();
		sampleSustain_.setText(stream.str());
		
		sampleRelease_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getRelease();
		sampleRelease_.setText(stream.str());

		sampleZoomInButton_.setDisabled(false);
		sampleZoomOutButton_.setDisabled(false);
			
		loopStart_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getLoopStart();
		loopStart_.setText(stream.str());
			
		loopEnd_.setDisabled(false);
		stream.str("");
		stream << GUIStuff->temporarySampleData_.getLoopEnd();
		loopEnd_.setText(stream.str());


		sampleGFXWindow_.setDisabled(false);
	}
	else
	{
		sampleList_.setSelected(false);

		rootNoteMenu_.setDisabled(true);
		lowNoteMenu_.setDisabled(true);
		highNoteMenu_.setDisabled(true);
		loopType_.setDisabled(true);
		sampleAttack_.setDisabled(true);
		sampleDecay_.setDisabled(true);
		sampleSustain_.setDisabled(true);
		sampleRelease_.setDisabled(true);
		sampleZoomInButton_.setDisabled(true);
		sampleZoomOutButton_.setDisabled(true);
		loopStart_.setDisabled(true);
		loopEnd_.setDisabled(true);
		sampleGFXWindow_.setDisabled(true);
	}
		
	localLockHierarchy.release();

	return true;
}


void InstrumentSettingsView::closeSampleMappingWindow()
{
	windowSystem_->closeWindow(&sampleMappingWindow_);
}


void InstrumentSettingsView::setAddSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	addSampleButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setRemoveSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	removeSampleButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleMappingCloseButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	sampleMappingCloseButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	sampleList_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setRootNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	rootNoteMenu_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setLowNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	lowNoteMenu_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setHighNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	highNoteMenu_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setLoopTypeCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	loopType_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleAttackCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	sampleAttack_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleDecayCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	sampleDecay_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleSustainCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	sampleSustain_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleReleaseCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	sampleRelease_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setPlaySampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	playSampleButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleZoomInButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	sampleZoomInButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setSampleZoomOutButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	sampleZoomOutButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setLoopStartCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	loopStart_.textEnter_.add(object, functionPointer);
}

void InstrumentSettingsView::setLoopEndCallback(void *object, void(*functionPointer)(void*, TextEvent*))
{
	loopEnd_.textEnter_.add(object, functionPointer);
}

// Add sample window

void InstrumentSettingsView::setAddSampleWindow()
{
	addSampleWindow_.setWidth(500);
	addSampleWindow_.setHeight(500);

	addSampleWindow_.addRegion("AddSampleLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		400, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	addSampleWindow_.addRegion("LoadedSamplesLabel",
		0, ATTACH_LEFT, "AddSampleLabel",
		10, ATTACH_BOTTOM, "AddSampleLabel",
		200, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	addSampleWindow_.addRegion("LoadedSamplesList",
		0, ATTACH_LEFT, "LoadedSamplesLabel",
		10, ATTACH_BOTTOM, "LoadedSamplesLabel",
		300, ATTACH_NONE, "",
		300, ATTACH_NONE, "");


	addSampleWindow_.addRegion("AddSampleOkButton",
		0, ATTACH_LEFT, "LoadedSamplesList",
		10, ATTACH_BOTTOM, "LoadedSamplesList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	addSampleWindow_.addRegion("AddSampleCancelButton",
		10, ATTACH_RIGHT, "AddSampleOkButton",
		0, ATTACH_TOP, "AddSampleOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	addSampleWindow_.addRegion("LoadSampleButton",
		10, ATTACH_RIGHT, "AddSampleCancelButton",
		0, ATTACH_TOP, "AddSampleCancelButton",
		100, ATTACH_NONE, "",
		30, ATTACH_NONE, "");



	/*
	Window addSampleWindow_;
	Label addSampleLabel_;
	Label loadedSamplesLabel_;
	List loadedSamplesList_;
	Button addSampleOkButton_, addSampleCancelButton_, loadSampleButton_;
	*/

	addSampleWindow_.setWidget("AddSampleLabel", &addSampleLabel_);
	addSampleWindow_.setWidget("LoadedSamplesLabel", &loadedSamplesLabel_);
	addSampleWindow_.setWidget("LoadedSamplesList", &loadedSamplesList_);
	addSampleWindow_.setWidget("AddSampleOkButton", &addSampleOkButton_);
	addSampleWindow_.setWidget("AddSampleCancelButton", &addSampleCancelButton_);
	addSampleWindow_.setWidget("LoadSampleButton", &loadSampleButton_);


	addSampleLabel_.text_ = "Add a sample?";
	loadedSamplesLabel_.text_ = "Loaded samples:";

	addSampleOkButton_.setName("Ok");
	addSampleCancelButton_.setName("Cancel");
	loadSampleButton_.setName("Load sample");
}

void InstrumentSettingsView::openAddSampleWindow()
{
	int x, y;

	windowSystem_->getTopWindowCoordinates(&x, &y);
	addSampleWindow_.setX(x + 30);
	addSampleWindow_.setY(y + 30);

	windowSystem_->addWindow(&addSampleWindow_);
}



bool InstrumentSettingsView::updateAddSampleWindow()
{
	if (instrumentSettingsModel_->sampleSelected())
	{
		loadedSamplesList_.setRow(instrumentSettingsModel_->getSelectedSampleIndex());
		addSampleOkButton_.setDisabled(false);
	}
	else
	{
		addSampleOkButton_.setDisabled(true);
	}

	return true;
	
}

void InstrumentSettingsView::closeAddSampleWindow()
{
	windowSystem_->closeWindow(&addSampleWindow_);
}

void InstrumentSettingsView::setAddSampleOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	addSampleOkButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setAddSampleCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	addSampleCancelButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setLoadSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	loadSampleButton_.onClick_.add(object, functionPointer);
}

void InstrumentSettingsView::setLoadedSamplesListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
{
	loadedSamplesList_.chooseEvent_.add(object, functionPointer);
}

void InstrumentSettingsView::openLoadSampleDialog()
{
	loadSampleFileWindowView_->open();
}


void InstrumentSettingsView::drawWaveForm(GraphicsWindow *window, int width, int height)
{
	bool success = localLockHierarchy.tryToAcquire(&instrumentSettingsViewLock_, 1, LOCK_LEVELS::GUI);
	
	if (success)
	{
		if (sampleCanvasUpdated_)
		{
			windowSystem_->attachWindowCanvas(waveFormCanvas_);
			sampleCanvasUpdated_ = false;
		}

		localLockHierarchy.release();
	}

	


	Rect dest;
	dest.x = 0;
	dest.y = 0;
	dest.width = width;
	dest.height = height;
	
	window->drawWindowSprite(waveFormSprite_, &dest, 0);
}

void InstrumentSettingsView::drawWaveForm_(void *object, GraphicsWindow *window, int width, int height)
{
	((InstrumentSettingsView*)object)->drawWaveForm(window, width, height);
}


// GUI thread
void InstrumentSettingsView::update(Subject *subject)
{
	COUNTED_THREAD thread(workerRefreshFromModel_, this);

	//updateGUI();
}

bool InstrumentSettingsView::updateGUI()
{

	bool error = false;
	if (!updateInstrumentSettingsView()) error = true;
	if (!updateNewInstrumentWindow()) error = true;
	//updateDeleteInstrumentWindow();

	if (!updateSampleMappingWindow()) error = true;
	if (!updateAddSampleWindow()) error = true;
	if (!updateDeleteInstrumentWindow()) error = true;

	
	return !error;
}

void InstrumentSettingsView::print()
{
	/*
	sampleNames_.switchData();
	instrumentNames_.switchData();

	std::vector<std::string> *names = sampleNames_.getData();


	LockGuard coutGuard(&localLockHierarchy, &coutLock_, 1, LOCK_LEVELS::IO);

	for (unsigned int index = 0; index < names->size(); index++)
	{
	std::cout << (*names)[index] << std::endl;
	}
	*/
}


bool InstrumentSettingsView::internalCallback(InstrumentSettingsViewInternalMessage *message)
{

	return updateGUI();
}


bool InstrumentSettingsView::internalCallback_(void *model, InstrumentSettingsViewInternalMessage *message)
{
	return ((InstrumentSettingsView*)model)->internalCallback(message);
}