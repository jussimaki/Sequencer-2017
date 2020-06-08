#include "tracks_edit_controller.h"

#include "tracks_edit_model.h"
#include "tracks_edit_view.h"


TracksEditController::TracksEditController(TracksEditModel *model, TracksEditView *view) : model_(model), view_(view)
{

	view_->setNewTrackButtonCallback(this, newTrackButtonClicked_);

}



void TracksEditController::newTrackButtonClicked(MouseEvent *event)
{
	model_->createNewTrack();
}

void TracksEditController::newTrackButtonClicked_(void *object, MouseEvent *event)
{
	((TracksEditController*)object)->newTrackButtonClicked(event);
}