#include "song_model.h"
#include "spawn_thread.h"

SongModel::SongModel(CASLock *songModelLock, CASLock *eventLock, IDIssuer <Track> *trackIDIssuer,
	CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer, int trackAmount, int phraseAmount, int midiEventAmount) :
	songModelLock_(songModelLock), eventLock_(eventLock), trackIDIssuer_(trackIDIssuer),
	songMessageRegisterer_(songMessageRegisterer)

{
	for (int index = 0; index < trackAmount; index++)
	{
		Node <Track *> *node = new Node <Track *>;


		node->data = new Track(trackIDIssuer_);

		trackPool_.addToBegin(node);
	}

}

void SongModel::createNewTrack()
{
	COUNTED_THREAD worker(workerCreateNewTrack_, this);
}