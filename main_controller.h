#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "main_view.h"

class InstrumentSettingsView;
class InstrumentSettingsController;
class Sequencer;

class MainController
{
public:
	MainController(Sequencer *sequencer, bool *quitSignal, MainView *mainView, InstrumentSettingsView *instrumentSettingsView);
	void setMainController();

	void newButtonClicked(MouseEvent *event);
	void saveButtonClicked(MouseEvent *event);
	void loadButtonClicked(MouseEvent *event);
	void quitButtonClicked(MouseEvent *event);

	void instrumentsButtonClicked(MouseEvent *event);
	void samplesButtonClicked(MouseEvent *event);
	void tracksButtonClicked(MouseEvent *event);

	void static newButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->newButtonClicked(event);
	}
	void static saveButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->saveButtonClicked(event);
	}
	void static loadButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->loadButtonClicked(event);
	}
	void static quitButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->quitButtonClicked(event);
	}

	void static instrumentsButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->instrumentsButtonClicked(event);
	}
	void static samplesButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->samplesButtonClicked(event);
	}
	void static tracksButtonClicked_(void *object, MouseEvent *event)
	{
		((MainController*)object)->tracksButtonClicked(event);
	}

private:
	MainView *mainView_;
	InstrumentSettingsView *instrumentSettingsView_;


	Sequencer *sequencer_;
	bool *quitSignal_;
};


#endif
