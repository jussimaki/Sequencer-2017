#include <windows.h>
#include "tracks_edit_model.h"
#include "song_model.h"
#include "spawn_thread.h"

TracksEditModel::TracksEditModel(CASLock *eventLock, CallbackConcreteRegisterer <TracksEditModelInternalMessage> *tracksEditModelInternalRegisterer,
	SongModel *songModel, CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer, CASLock *tracksEditModelLock, CASLock *songModelLock) :

	eventLock_(eventLock), tracksEditModelInternalRegisterer_(tracksEditModelInternalRegisterer), songModel_(songModel),
	songMessageRegisterer_(songMessageRegisterer), tracksEditModelLock_(tracksEditModelLock), songModelLock_(songModelLock), trackAmount_(0)
{
	tracksEditModelInternalRegisterer_->registerListener(this, internalCallback_);
	songMessageRegisterer_->registerListener(this, songMessageCallback_);
}

TracksEditModel::~TracksEditModel()
{
	songMessageRegisterer_->unregisterListener(this);
	tracksEditModelInternalRegisterer_->unregisterListener(this);
}

void TracksEditModel::registerObserver(Observer *observer)
{
	subject_.registerObserver(observer);
}

void TracksEditModel::unregisterObserver(Observer *observer)
{
	subject_.unregisterObserver(observer);
}

bool TracksEditModel::internalCallback(TracksEditModelInternalMessage *message)
{
	switch (message->type_)
	{
	case TracksEditModelInternalMessage::MessageType::TRACK_ADDED:
		trackAmount_ += message->amount_;

		subject_.notify();
		break;
	default:
		break;
	}

	return true;
}

bool TracksEditModel::internalCallback_(void *model, TracksEditModelInternalMessage *message)
{
	return ((TracksEditModel*)model)->internalCallback(message);
}


void TracksEditModel::createNewTrack()
{
	songModel_->createNewTrack();
}

int TracksEditModel::getTrackAmount()
{
	return trackAmount_;
}

bool TracksEditModel::songMessageCallback(SongMessage *message)
{
	if (message->type_ == SongMessage::MessageType::TRACK_ADDED)
	{
		
		COUNTED_THREAD worker(workerTrackAdded_, this);

	}


	return true;
}

bool TracksEditModel::songMessageCallback_(void *model, SongMessage *message)
{
	return ((TracksEditModel*)model)->songMessageCallback(message);
}