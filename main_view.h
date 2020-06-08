#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include "window.h"
#include "observer.h"

class InstrumentSampleModel;
class TracksEditView;

class MainView : public Observer
{
public:
	MainView(InstrumentSampleModel *instrumentSampleModel, WindowSystem *windowSystem, TracksEditView *tracksEditView);
	~MainView();
	void setMainView();
	void openMainView();
	void update(Subject *subject);

	void setNewButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		newButton_.onClick_.add(object, functionPointer);
	}

	void setSaveButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		saveButton_.onClick_.add(object, functionPointer);
	}

	void setLoadButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		loadButton_.onClick_.add(object, functionPointer);
	}

	void setQuitButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		quitButton_.onClick_.add(object, functionPointer);
	}

	void setInstrumentsButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		instrumentsButton_.onClick_.add(object, functionPointer);
	}

	void setSamplesButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		samplesButton_.onClick_.add(object, functionPointer);
	}

	void setTracksButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		tracksButton_.onClick_.add(object, functionPointer);
	}

private:
	WindowSystem *windowSystem_;

	Window mainWindow_;
	Button newButton_, saveButton_, loadButton_, quitButton_;
	Button instrumentsButton_, samplesButton_, tracksButton_;

	InstrumentSampleModel *instrumentSampleModel_;

	TracksEditView *tracksEditView_;
};

#endif