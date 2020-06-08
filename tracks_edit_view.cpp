#include <Windows.h>

#include "lock_guard.h"
#include "lock_levels.h"

#include "tracks_edit_view.h"
#include "tracks_edit_model.h"

extern thread_local LockHierarchy localLockHierarchy;
extern int globalInt;

void TracksEditView::workerRefreshFromModel()
{


	LockGuard tracksEditViewGuard(&localLockHierarchy, &tracksEditViewLock_, 1, LOCK_LEVELS::GUI);

	LockGuard tracksEditModelGuard(&localLockHierarchy, &tracksEditModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

	int trackAmount = tracksEditModel_->getTrackAmount();

	drawTracksToCanvases();

	// create new graphicwindows
	std::vector <GraphicsWindow *> newGFXWindows;


	
	int amount = 0;

	if (trackAmount > GFXWindowsAmount_)
	{
		amount = trackAmount -GFXWindowsAmount_;
		
		for (int index = 0; index < amount; index++)
		{
			GraphicsWindow *window = new GraphicsWindow;
			newGFXWindows.push_back(window);
		}

	}





	TracksEditViewInternalMessage *message = new TracksEditViewInternalMessage;

	if(amount > 0)
	{
		message->type_ = TracksEditViewInternalMessage::MessageType::NEW_GRAPHICSWINDOWS;
		newGFXWindows_ = newGFXWindows;
	}
	else message->type_ = TracksEditViewInternalMessage::MessageType::DATA_REFRESHED;


	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	tracksEditViewInternalRegisterer_->notify(message);

}

void TracksEditView::workerRefreshFromModel_(TracksEditView *view)
{
	view->workerRefreshFromModel();

}


void TracksEditView::workerCreateNewGFXWindow()
{

}

void TracksEditView::workerCreateNewGFXWindow_(TracksEditView *view)
{
	view->workerCreateNewGFXWindow();
}


// tracksEditViewLock_ and tracksEditModelLock_ must be acquired to call
// this function!!!
void TracksEditView::drawTracksToCanvases()
{
	std::vector<EditTrack *> *tracks = tracksEditModel_->getTracks();

	for (int index = 0; index < tracks->size(); index++)
	{
		if (trackCanvases_.size() <= index) break;
		
		WindowCanvas *canvas = &(trackCanvases_[index]);

		for (int x = 0; x < canvas->width; x++)
		{
			for (int y = 0; y < canvas->height; y++)
			{
				canvas->data[(y*canvas->width + x) * 4] = 2555;// rand() % 256;
				canvas->data[(y*canvas->width + x) * 4 + 1] = 128;// rand() % 256;
				canvas->data[(y*canvas->width + x) * 4 + 2] = 0;// rand() % 256;
				canvas->data[(y*canvas->width + x) * 4 + 3] = 255;
			}

		}

	}

}