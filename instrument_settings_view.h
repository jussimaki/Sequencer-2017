#ifndef INSTRUMENT_SETTINGS_VIEW_H
#define INSTRUMENT_SETTINGS_VIEW_H

#include <vector>

#include "callback_registerer.h"

#include "observer.h"
#include "switchable.h"
#include "window.h"

#include "ok_cancel_dialog.h"
#include "instrument_sample_model.h"

#include "file_window_view.h"


class CASLock;
class InstrumentSettingsModel;

class InstrumentSettingsViewInternalMessage
{
public:
	InstrumentSettingsViewInternalMessage() :
		instrumentSelected(false),
		sampleDataSelected(false),
		sampleSelected(false),
		instrumentIndex(0),
		sampleDataIndex(0),
		sampleIndex(0),
		instrumentName(""),
		type_(InstrumentSettingsViewInternalMessage::MessageType::DATA_REFRESHED)
		{}

	enum MessageType { DATA_REFRESHED };


	bool instrumentSelected, sampleDataSelected, sampleSelected;
	int instrumentIndex, sampleDataIndex, sampleIndex;
	std::string instrumentName;
	MessageType type_;
};


class InstrumentSettingsView : public Observer
{
public:
	InstrumentSettingsView(CASLock *instrumentSettingsModelLock, CASLock *instrumentSettingsViewLock, CASLock *eventLock, CASLock *coutLock,
		CallbackConcreteRegisterer<InstrumentSettingsViewInternalMessage> *instrumentSettingsViewInternalRegisterer,
		IDIssuer <Instrument> *instrumentIDIssuer, IDIssuer <Sample> *sampleIDIssuer,
		InstrumentSettingsModel *instrumentSettingsModel, WindowSystem *windowSystem,
		FileWindowView *loadSampleFileWindowView);

	~InstrumentSettingsView();


	// GUI thread
	bool internalCallback(InstrumentSettingsViewInternalMessage *message);
	bool static internalCallback_(void *view, InstrumentSettingsViewInternalMessage *message);

	void update(Subject *subject);
	bool updateGUI();
	void print();


	// workers
	void workerRefreshFromModel();
	void static workerRefreshFromModel_(InstrumentSettingsView *view);

	


	// GUI functions stuff

	// Later
	//FileWindowView *getLoadSampleFileWindowView() { return &loadSampleFileWindowView_; }
	
	// Instrument settings window

	void setInstrumentSettingsView();
	void openInstrumentSettingsView();
	void closeInstrumentSettingsView();
	bool updateInstrumentSettingsView();

	void setInstrumentListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setSampleMappingButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setNewInstrumentButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setDeleteInstrumentButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setCloseButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	// New instrument window

	void setNewInstrumentWindow();
	void openNewInstrumentWindow();
	void closeNewInstrumentWindow();
	bool updateNewInstrumentWindow();

	void setNewInstrumentNameCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setNewInstrumentTypeCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setNewInstrumentOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setNewInstrumentCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	// Delete instrument window

	void openDeleteInstrumentWindow();
	bool updateDeleteInstrumentWindow();

	void closeDeleteInstrumentWindow();
	void setDeleteInstrumentOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setDeleteInstrumentCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setDeleteInstrumentMessage(std::string message);

	// Sample mapping window

	void setSampleMappingWindow();
	void openSampleMappingWindow();
	bool updateSampleMappingWindow();

	void closeSampleMappingWindow();


	void setAddSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	void setRemoveSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	void setSampleMappingCloseButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	void setSampleListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setRootNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setLowNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setHighNoteCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setLoopTypeCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));
	void setSampleAttackCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setSampleDecayCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setSampleSustainCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setSampleReleaseCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setPlaySampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setSampleZoomInButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setSampleZoomOutButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setLoopStartCallback(void *object, void(*functionPointer)(void*, TextEvent*));
	void setLoopEndCallback(void *object, void(*functionPointer)(void*, TextEvent*));



	// Add sample window

	void setAddSampleWindow();
	void openAddSampleWindow();
	bool updateAddSampleWindow();

	void closeAddSampleWindow();
	void setAddSampleOkButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setAddSampleCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setLoadSampleButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));
	void setLoadedSamplesListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*));

	void openLoadSampleDialog();




private:
	
	CASLock *instrumentSettingsModelLock_, *instrumentSettingsViewLock_, *eventLock_, *coutLock_;


	CallbackConcreteRegisterer<InstrumentSettingsViewInternalMessage> *instrumentSettingsViewInternalRegisterer_;
	InstrumentSettingsModel *instrumentSettingsModel_;

	WindowSystem *windowSystem_;


	// worker thread functions
	// instrumentSettingsViewLock_ must be acquired to call these functions!!
	void drawWaveFormToCanvas();

	// worker thread stuff
	// protected under instrumentSettingsViewLock_
	
	Sample sampleDataSample_;
	WindowCanvas waveFormCanvas_;
	bool sampleCanvasUpdated_;
	



	// GUI thread stuff

	
	// instrument settings window

	Window instrumentSettingsWindow_;
	Label instrumentLabel_;
	List instrumentList_;
	Button sampleMappingButton_;
	Button newInstrumentButton_, deleteInstrumentButton_;
	Button closeButton_;

	// new instrument window

	Window newInstrumentWindow_;
	Label newInstrumentLabel_;
	Label newInstrumentNameLabel_;
	TextField newInstrumentNameTextField_;
	Label newInstrumentTypeLabel_;
	RadioButtons newInstrumentTypeRadioButtons_;
	Button newInstrumentOkButton_;
	Button newInstrumentCancelButton_;

	// delete instrument window

	OkCancelDialog deleteInstrumentDialog_;


	// Sample mapping window

	Window sampleMappingWindow_;
	Label sampleMappingLabel_;
	Label samplesLabel_;


	List sampleList_;
	Button addSampleButton_;

	Button removeSampleButton_;
	Button sampleMappingCloseButton_;
	Label sampleMappingSampleName_;
	Label sampleLength_;
	Label rootNoteLabel_, lowNoteLabel_, highNoteLabel_;
	DropMenu rootNoteMenu_, lowNoteMenu_, highNoteMenu_;

	Label loopTypeLabel_;
	RadioButtons loopType_;

	Label sampleAttackLabel_, sampleDecayLabel_, sampleSustainLabel_, sampleReleaseLabel_;
	TextField sampleAttack_, sampleDecay_, sampleSustain_, sampleRelease_;
	Button playSampleButton_;

	Button sampleZoomInButton_, sampleZoomOutButton_;
	Label loopStartLabel_, loopEndLabel_;
	TextField loopStart_, loopEnd_;

	GraphicsWindow sampleGFXWindow_;

	WindowSprite waveFormSprite_;

	void drawWaveForm(GraphicsWindow *window, int width, int height);
	void static drawWaveForm_(void *object, GraphicsWindow *window, int width, int height);

	// Add sample window

	Window addSampleWindow_;
	Label addSampleLabel_;
	Label loadedSamplesLabel_;
	List loadedSamplesList_;
	Button addSampleOkButton_, addSampleCancelButton_, loadSampleButton_;


	Window removeSampleWindow_;
	Label removeSampleLabel_;
	Button removeSampleOkButton_, removeSampleCancelButton_;

	FileWindowView *loadSampleFileWindowView_;

};


#endif
