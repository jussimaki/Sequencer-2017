#include "tracks_edit_model.h"

#include "lock_levels.h"
#include "lock_guard.h"
#include "song_model.h"


extern thread_local LockHierarchy localLockHierarchy;

void TracksEditModel::workerTrackAdded()
{
	LockGuard tracksEditGuard(&localLockHierarchy, &tracksEditModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);
	LockGuard songGuard(&localLockHierarchy, &songModelLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);	

	Pool <Track *> *tracks= songModel_->getTracks();

	int count = 0;

	Node <Track *> *beginNode = tracks->getFirstNode();

	while (beginNode)
	{
		count++;
		beginNode = beginNode->next;
	}


	int trackAmount = tracks_.size();
		
	for (int index = 0; index < count - trackAmount; index++)
	{
		EditTrack *track = new EditTrack;
		tracks_.push_back(track);
	}

	TracksEditModelInternalMessage *message = new TracksEditModelInternalMessage;

	message->amount_ = count - trackAmount;
	message->type_ = TracksEditModelInternalMessage::MessageType::TRACK_ADDED;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

	tracksEditModelInternalRegisterer_->notify(message);
	

}

void TracksEditModel::workerTrackAdded_(TracksEditModel *model)
{
	model->workerTrackAdded();
}


std::vector<EditTrack *> *TracksEditModel::getTracks()
{
	return &tracks_;
}