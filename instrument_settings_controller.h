#ifndef INSTRUMENT_SETTINGS_CONTROLLER_H
#define INSTRUMENT_SETTINGS_CONTROLLER_H

//#include "file_window_controller.h"

#include "window.h"

class InstrumentSettingsModel;
class InstrumentSettingsView;
class FileWindowView;




class InstrumentSettingsController
{
public:
	InstrumentSettingsController(InstrumentSettingsModel *instrumentSettingsModel, InstrumentSettingsView *instrumentSettingsView);
	~InstrumentSettingsController();
	
	void setInstrumentSettingsController();

	// Instrument settings window

	void instrumentListClicked(ChooseEvent *event);
	void static instrumentListClicked_(void *object, ChooseEvent *event);
	void sampleMappingButtonClicked(MouseEvent *event);
	void static sampleMappingButtonClicked_(void *object, MouseEvent *event);
	void newInstrumentButtonClicked(MouseEvent *event);
	void static newInstrumentButtonClicked_(void *object, MouseEvent *event);
	void deleteInstrumentButtonClicked(MouseEvent *event);
	void static deleteInstrumentButtonClicked_(void *object, MouseEvent *event);
	void closeButtonClicked(MouseEvent *event);
	void static closeButtonClicked_(void *object, MouseEvent *event);
	

	// New instrument window

	void newInstrumentNameChanged(TextEvent *event);
	void static newInstrumentNameChanged_(void *object, TextEvent *event);
	void newInstrumentTypeChanged(ChooseEvent *event);
	void static newInstrumentTypeChanged_(void *object, ChooseEvent *event);
	void newInstrumentOkButtonClicked(MouseEvent *event);
	void static newInstrumentOkButtonClicked_(void *object, MouseEvent *event);
	void newInstrumentCancelButtonClicked(MouseEvent *event);
	void static newInstrumentCancelButtonClicked_(void *object, MouseEvent *event);
	

	// Delete instrument window

	void deleteInstrumentOkButtonClicked(MouseEvent *event);
	void static deleteInstrumentOkButtonClicked_(void *object, MouseEvent *event);
	void deleteInstrumentCancelButtonClicked(MouseEvent *event);
	void static deleteInstrumentCancelButtonClicked_(void *object, MouseEvent *event);


	// Sample mapping window

	void addSampleButtonClicked(MouseEvent *event);
	void static addSampleButtonClicked_(void *object, MouseEvent *event);
	void removeSampleButtonClicked(MouseEvent *event);
	void static removeSampleButtonClicked_(void *object, MouseEvent *event);
	void sampleMappingCloseButtonClicked(MouseEvent *event);
	void static sampleMappingCloseButtonClicked_(void *object, MouseEvent *event);
	void sampleListCallback(ChooseEvent *event);
	void static sampleListCallback_(void *object, ChooseEvent *event);
	void rootNoteCallback(ChooseEvent *event);
	void static rootNoteCallback_(void *object, ChooseEvent *event);
	void lowNoteCallback(ChooseEvent *event);
	void static lowNoteCallback_(void *object, ChooseEvent *event);
	void highNoteCallback(ChooseEvent *event);
	void static highNoteCallback_(void *object, ChooseEvent *event);
	void loopTypeCallback(ChooseEvent *event);
	void static loopTypeCallback_(void *object, ChooseEvent *event);
	void attackCallback(TextEvent *event);
	void static attackCallback_(void *object, TextEvent *event);
	void decayCallback(TextEvent *event);
	void static decayCallback_(void *object, TextEvent *event);
	void sustainCallback(TextEvent *event);
	void static sustainCallback_(void *object, TextEvent *event);
	void releaseCallback(TextEvent *event);
	void static releaseCallback_(void *object, TextEvent *event);
	void playSampleButtonClicked(MouseEvent *event);
	void static playSampleButtonClicked_(void *object, MouseEvent *event);
	void zoomInButtonClicked(MouseEvent *event);
	void static zoomInButtonClicked_(void *object, MouseEvent *event);
	void zoomOutButtonClicked(MouseEvent *event);
	void static zoomOutButtonClicked_(void *object, MouseEvent *event);
	void loopStartCallback(TextEvent *event);
	void static loopStartCallback_(void *object, TextEvent *event);
	void loopEndCallback(TextEvent *event);
	void static loopEndCallback_(void *object, TextEvent *event);
	

	// Add sample window

	void addSampleOkButtonClicked(MouseEvent *event);
	void static addSampleOkButtonClicked_(void *object, MouseEvent *event);
	void addSampleCancelButtonClicked(MouseEvent *event);
	void static addSampleCancelButtonClicked_(void *object, MouseEvent *event);
	void loadSampleButtonClicked(MouseEvent *event);
	void static loadSampleButtonClicked_(void *object, MouseEvent *event);
	void loadedSamplesListClicked(ChooseEvent *event);
	void static loadedSamplesListClicked_(void *object, ChooseEvent *event);
	
private:
	InstrumentSettingsModel *instrumentSettingsModel_;
	InstrumentSettingsView *instrumentSettingsView_;

	

};

#endif