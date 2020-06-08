#ifndef TRACKS_EDIT_MODEL_H
#define TRACKS_EDIT_MODEL_H

#include <vector>
#include "id.h"
#include "callback_registerer.h"
#include "song_model.h"
#include "subject.h"

/*
struct Note
{
	unsigned char value_;
	
	unsigned char velocity_;
	int beginPulse_;
	int lengthInPulses_;

};
*/

class EditPhrase
{
	int beginBar_, beginBeat_;
	int endBar_, endBeat_;

};

class EditTrack
{
public:
	EditTrack() : name_(""), trackID_(0) {}


private:
	std::string name_;

	ID <EditTrack> trackID_;


	std::vector <EditPhrase> phrases_;
};

class SongModel;

class TracksEditModelInternalMessage
{
public:
	TracksEditModelInternalMessage() : amount_(0) {}
	

	enum MessageType{ TRACK_ADDED};
		
	MessageType type_;
	int amount_;
};

class CASLock;

class TracksEditModel
{

public:

	TracksEditModel(CASLock *eventLock, CallbackConcreteRegisterer <TracksEditModelInternalMessage> *tracksEditModelInternalRegisterer,
		SongModel *songModel, CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer, CASLock *tracksEditModelLock, CASLock *songModelLock);
	~TracksEditModel();

	void registerObserver(Observer *observer);
	void unregisterObserver(Observer *observer);

	// GUI thread callbacks
	bool internalCallback(TracksEditModelInternalMessage *message);
	bool static internalCallback_(void *model, TracksEditModelInternalMessage *message);

	// GUI Stuff
	void createNewTrack();
	int getTrackAmount();

	bool songMessageCallback(SongMessage *message);
	bool static songMessageCallback_(void *model, SongMessage *message);

	// workers

	void workerTrackAdded();
	void static workerTrackAdded_(TracksEditModel *model);


	// worker thread functions

	std::vector<EditTrack *> *getTracks();




private:

	SongModel *songModel_;
	CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer_;
	CASLock  *eventLock_;
	CallbackConcreteRegisterer <TracksEditModelInternalMessage> *tracksEditModelInternalRegisterer_;
	CASLock *tracksEditModelLock_, *songModelLock_;

	// GUI thread data

	Subject subject_;
	int trackAmount_;

	// Slow data, protected under trackEditModelLock_!!
	std::vector<EditTrack *> tracks_;

	int barAmount_;
	double tempo_;

	int currentBar_;
	int currentBeat_;
	int currentPulse_;



};

#endif
