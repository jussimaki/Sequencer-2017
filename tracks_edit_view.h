#ifndef TRACKS_EDIT_VIEW_H
#define TRACKS_EDIT_VIEW_H

#include "window.h"
#include "callback_registerer.h"
#include "observer.h"

class TracksEditViewInternalMessage
{
public:
	TracksEditViewInternalMessage() :
		
		type_(TracksEditViewInternalMessage::MessageType::DATA_REFRESHED)
	{}

	enum MessageType { DATA_REFRESHED, NEW_GRAPHICSWINDOWS };

	MessageType type_;
};




class TracksEditModel;
class CASLock;


class TracksEditView : public Observer
{
public:
	TracksEditView(CASLock *tracksEditViewLock, CASLock *tracksEditModelLock, CASLock *eventLock, CallbackConcreteRegisterer<TracksEditViewInternalMessage> *tracksEditViewInternalRegisterer, TracksEditModel *tracksEditModel, WindowSystem *windowSystem);
	~TracksEditView();



	// GUI thread
	bool updateGraphics();
	bool updateGUI();
	bool internalCallback(TracksEditViewInternalMessage *message);
	bool static internalCallback_(void *view, TracksEditViewInternalMessage *message);

	void update(Subject *subject);

	void setTracksEditView();
	bool updateTracksEditView();

	void setNewTrackButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*));

	void trackMouseCallback(MouseEvent* event);
	static void trackMouseCallback_(void *object, MouseEvent* event);


	void drawTrack(GraphicsWindow *window, int width, int height);
	static void drawTrack_(void *object, GraphicsWindow *window, int width, int height);

	// workers
	void workerRefreshFromModel();
	void static workerRefreshFromModel_(TracksEditView *view);
	
	void workerCreateNewGFXWindow();
	void static workerCreateNewGFXWindow_(TracksEditView *view);

	// worker thread functions
	void drawTracksToCanvases();



	Window *getTracksEditWindow();
private:

	CASLock *tracksEditViewLock_, *tracksEditModelLock_, *eventLock_;
	CallbackConcreteRegisterer<TracksEditViewInternalMessage> *tracksEditViewInternalRegisterer_;

	TracksEditModel *tracksEditModel_;
	WindowSystem *windowSystem_;

	// GUI Data

	Window tracksEditWindow_;
	Button newTrackButton_;


	std::vector <GraphicsWindow *> GFXWindows_;
	std::vector <WindowSprite> trackSprites_;
	
	// slow data!! protected under tracksViewLock

	std::vector <GraphicsWindow *> newGFXWindows_;
	int GFXWindowsAmount_;

	
	std::vector <WindowCanvas> trackCanvases_;
	
};


#endif