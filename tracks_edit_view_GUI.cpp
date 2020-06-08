#include <sstream>

#include "tracks_edit_view.h"
#include "tracks_edit_model.h"
#include "spawn_thread.h"

extern std::string debugString;


extern int globalInt, globalInt2, globalInt3;

TracksEditView::TracksEditView(CASLock *tracksEditViewLock, CASLock *tracksEditModelLock, CASLock *eventLock, CallbackConcreteRegisterer<TracksEditViewInternalMessage> *tracksEditViewInternalRegisterer, TracksEditModel *tracksEditModel, WindowSystem *windowSystem) :
	tracksEditViewLock_(tracksEditViewLock), tracksEditModelLock_(tracksEditModelLock), eventLock_(eventLock), tracksEditViewInternalRegisterer_(tracksEditViewInternalRegisterer), tracksEditModel_(tracksEditModel), windowSystem_(windowSystem), GFXWindowsAmount_(0)
{
	tracksEditViewInternalRegisterer_->registerListener(this, internalCallback_);
	tracksEditModel_->registerObserver(this);
	setTracksEditView();
}

TracksEditView::~TracksEditView()
{
	tracksEditModel_->unregisterObserver(this);
	tracksEditViewInternalRegisterer_->unregisterListener(this);
}




bool TracksEditView::updateGUI()
{
	bool error = false;

	if (!updateTracksEditView()) error = true;

	return !error;
}

bool TracksEditView::internalCallback(TracksEditViewInternalMessage *message)
{
	if (message->type_ == TracksEditViewInternalMessage::MessageType::NEW_GRAPHICSWINDOWS)
	{
		bool success = localLockHierarchy.tryToAcquire(&tracksEditViewLock_, 1, LOCK_LEVELS::GUI);

		if (!success) return false;

		int amount = newGFXWindows_.size();


		GFXWindowsAmount_ += newGFXWindows_.size();

		
		for (int index = 0; index < newGFXWindows_.size(); index++)
		{
			GFXWindows_.push_back(newGFXWindows_[index]);
		}

		newGFXWindows_.clear();
		
		


		for (int index = 0; index < amount; index++)
		{
			WindowSprite sprite = windowSystem_->getEmptyWindowSprite(1000, 300);
			WindowCanvas canvas = windowSystem_->getWindowCanvas(sprite);

			trackSprites_.push_back(sprite);
			trackCanvases_.push_back(canvas);

		}

		localLockHierarchy.release();
	}
	

	
	return updateGUI();
}

bool TracksEditView::internalCallback_(void *view, TracksEditViewInternalMessage *message)
{
	return ((TracksEditView*)view)->internalCallback(message);
}

void TracksEditView::setTracksEditView()
{
	std::stringstream globalStream;

	for (int track = 0; track < tracksEditModel_->getTrackAmount(); track++)
	{
		std::stringstream stream;

		stream << "TrackRegion";
		stream << track;
		
		std::string name = stream.str();
		
		std::string name2 = "";
		bool nameIsNone = true;

		if (track > 0)
		{
			nameIsNone = false;
			std::stringstream stream2;
			
			stream2 << "TrackRegion";
			stream2 << (track -1);

			name2 = stream2.str();

		}


		globalStream << " (" << name << " , " << name2 << ") ";

		if(nameIsNone)
			tracksEditWindow_.addRegion(name, 0, ATTACH_NONE, name2,
				0, ATTACH_NONE, name2,
				0, ATTACH_RIGHT, "",
				100, ATTACH_NONE, "");
		else tracksEditWindow_.addRegion(name, 0, ATTACH_LEFT, name2,
			10, ATTACH_BOTTOM, name2,
			0, ATTACH_RIGHT, "",
			100, ATTACH_NONE, "");

	}


	if (tracksEditModel_->getTrackAmount() > 0)
	{
		std::stringstream stream3;

		stream3 << "TrackRegion";
		stream3 << (tracksEditModel_->getTrackAmount() - 1);

		std::string name = stream3.str();

		tracksEditWindow_.addRegion("NewTrackButton", 0, ATTACH_LEFT, name,
			10, ATTACH_BOTTOM, name,
			100, ATTACH_NONE, "",
			60, ATTACH_NONE, "");
	}
	else tracksEditWindow_.addRegion("NewTrackButton", 0, ATTACH_NONE, "",
		10, ATTACH_NONE, "",
		100, ATTACH_NONE, "",
		60, ATTACH_NONE, "");
		
	
	
	

	tracksEditWindow_.setWidget("NewTrackButton", &newTrackButton_);

	newTrackButton_.setName("New track");

	tracksEditWindow_.generalMouseEvent_.add(this, trackMouseCallback_);
}


bool TracksEditView::updateTracksEditView()
{
	//bool success = localLockHierarchy.tryToAcquire(&tracksEditViewLock_, 1, LOCK_LEVELS::GUI);
	
	//if (!success) return false;

	tracksEditWindow_.clearRegions();
	std::stringstream globalStream;
	
	for (int track = 0; track < tracksEditModel_->getTrackAmount(); track++)
	{
		std::stringstream stream;

		stream << "TrackRegion";
		stream << track;

		std::string name = stream.str();

		std::string name2 = "";
		bool nameIsNone = true;

		if (track > 0)
		{
			nameIsNone = false;
			std::stringstream stream2;

			stream2 << "TrackRegion";
			stream2 << (track - 1);

			name2 = stream2.str();

		}


		globalStream << " (" << name << " , " << name2 << ") ";

		if (nameIsNone)
			tracksEditWindow_.addRegion(name, 0, ATTACH_NONE, name2,
				0, ATTACH_NONE, name2,
				0, ATTACH_RIGHT, "",
				100, ATTACH_NONE, "");
		else tracksEditWindow_.addRegion(name, 0, ATTACH_LEFT, name2,
			10, ATTACH_BOTTOM, name2,
			0, ATTACH_RIGHT, "",
			100, ATTACH_NONE, "");


		if (GFXWindows_.size() > track)
		{
			tracksEditWindow_.setWidget(name, GFXWindows_[track]);

			GFXWindows_[track]->clearDrawFunctions();
			GFXWindows_[track]->addDrawFunction(this, drawTrack_);

			
		}
	}


	if (tracksEditModel_->getTrackAmount() > 0)
	{
		std::stringstream stream3;

		stream3 << "TrackRegion";
		stream3 << (tracksEditModel_->getTrackAmount() - 1);

		std::string name = stream3.str();

		tracksEditWindow_.addRegion("NewTrackButton", 0, ATTACH_LEFT, name,
			10, ATTACH_BOTTOM, name,
			100, ATTACH_NONE, "",
			60, ATTACH_NONE, "");
	}
	else tracksEditWindow_.addRegion("NewTrackButton", 0, ATTACH_NONE, "",
		10, ATTACH_NONE, "",
		100, ATTACH_NONE, "",
		60, ATTACH_NONE, "");





	tracksEditWindow_.setWidget("NewTrackButton", &newTrackButton_);

	newTrackButton_.setName("New track");


	//localLockHierarchy.release();

	return true;
}

void TracksEditView::update(Subject *subejct)
{
	COUNTED_THREAD thread(workerRefreshFromModel_, this);

}

void TracksEditView::setNewTrackButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
{
	newTrackButton_.onClick_.add(object, functionPointer);
}


void TracksEditView::trackMouseCallback(MouseEvent* event)
{

}

void TracksEditView::trackMouseCallback_(void *object, MouseEvent* event)
{
	((TracksEditView*)object)->trackMouseCallback(event);
}



void TracksEditView::drawTrack(GraphicsWindow *window, int width, int height)
{
	int rightIndex = 0;
	bool found = false;

	for (int index = 0; index < GFXWindows_.size(); index++)
	{
		if (GFXWindows_[index] == window) {
			rightIndex = index; found = true;
						
		}
	}

	if (!found) return;

	if (trackSprites_.size() <= rightIndex) return;
	

	bool success = localLockHierarchy.tryToAcquire(&tracksEditViewLock_, 1, LOCK_LEVELS::GUI);
	
	if (success)
	{
		if (rightIndex < trackCanvases_.size())
		{
			WindowCanvas canvas = trackCanvases_[rightIndex];
			
			windowSystem_->attachWindowCanvas(canvas);
		}
				
		localLockHierarchy.release();
	}
	


	Rect dest;
	dest.x = 0;
	dest.y = 0;
	dest.width = width;
	dest.height = height;

	window->drawWindowSprite(trackSprites_[rightIndex], &dest, 0);
}

void TracksEditView::drawTrack_(void *object, GraphicsWindow *window, int width, int height)
{
	((TracksEditView*)object)->drawTrack(window, width, height);
}

Window *TracksEditView::getTracksEditWindow() { return &tracksEditWindow_; }