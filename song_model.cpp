#include "song_model.h"


extern thread_local LockHierarchy localLockHierarchy;


void Track::addPhrase(Node <Phrase*> *phraseNode)
{
	phrases_.addToEnd(phraseNode);
}

void SongModel::workerCreateNewTrack()
{
	//Track *track = new Track(trackIDIssuer_);

	//Node <Track*> *node = new Node <Track*>;

	Node <Track*> *node = trackPool_.extractFirstNode();

	if (node)
	{
		tracks_.addToEnd(node);



		LockGuard songGuard(&localLockHierarchy, &songModelLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);



		SongMessage *message = new SongMessage;

		message->trackID_ = node->data->getID();
		message->type_ = SongMessage::MessageType::TRACK_ADDED;


		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

		songMessageRegisterer_->notify(message);



	}

}

void SongModel::workerCreateNewTrack_(SongModel *model)
{
	model->workerCreateNewTrack();
}


void SongModel::workerCreateNewPhrase(ID <Track> trackID)
{
	LockGuard songGuard(&localLockHierarchy, &songModelLock_, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

	Node <Track*> *node = tracks_.getFirstNode();

	ID <Phrase> ID(0);
	bool found = false;

	while (node)
	{
		Track *track = node->data;

		if (track->getID() == trackID)
		{
			Node <Phrase*> *phraseNode = phrasePool_.extractFirstNode();

			if (phraseNode)
			{
				found = true;
				ID = phraseNode->data->getID();
				track->addPhrase(phraseNode);
			}
		}

		node = node->next;
	}


	if (found)
	{

		SongMessage *message = new SongMessage;

		message->phraseID_ = ID;
		message->type_ = SongMessage::MessageType::PHRASE_ADDED;


		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

		songMessageRegisterer_->notify(message);

	}
}

void SongModel::workerCreateNewPhrase(SongModel *songModel, ID <Track> trackID)
{
	songModel->workerCreateNewPhrase(trackID);
}

Pool <Track *> *SongModel::getTracks()
{
	return &tracks_;
}