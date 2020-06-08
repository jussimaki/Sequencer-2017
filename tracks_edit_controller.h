#ifndef TRACKS_EDIT_CONTROLLER_H
#define TRACKS_EDIT_CONTROLLER_H

#include "window.h"

class TracksEditModel;
class TracksEditView;

class TracksEditController
{
public:
	TracksEditController(TracksEditModel *model, TracksEditView *view);

	void newTrackButtonClicked(MouseEvent *event);
	void static newTrackButtonClicked_(void *object, MouseEvent *event);


private:

	TracksEditModel *model_;
	TracksEditView *view_;
};

#endif
