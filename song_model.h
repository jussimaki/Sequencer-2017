#ifndef SONG_MODEL_H
#define SONG_MODEL_H

#include "id.h"
#include "callback_registerer.h"
#include "pool.h"

struct MidiEvent
{
	char type;
	char value;
	char velocity;
	int beginPulse;
	int lenghtInPulses;
};

class Phrase
{
public:
	Phrase(IDIssuer <Phrase> *phraseIDIssuer) : ID_(phraseIDIssuer)
	{
		ID_.acquireID();
	}

	ID <Phrase> getID() { return ID_; }

private:
	ID <Phrase> ID_;
	Pool <MidiEvent *> midiEvents_;
};

class Track
{
public:
	Track(IDIssuer <Track> *trackIDIssuer) : ID_(trackIDIssuer)
	{
		ID_.acquireID();
	}

	ID <Track> getID() { return ID_; }

	void addPhrase(Node <Phrase*> *phraseNode);

private:
	ID <Track> ID_;
	Pool <Phrase *> phrases_;
};

class CASLock;




class SongMessage
{
public:
	SongMessage() : trackID_(0), phraseID_(0) {}
	enum MessageType { TRACK_ADDED, TRACK_DELETED, PHRASE_ADDED, PHRASE_DELETED, PHRASE_CHANGED };

	MessageType type_;
	ID <Track> trackID_;
	ID <Phrase> phraseID_;
};


class SongModel
{
public:
	SongModel(CASLock *songModelLock, CASLock *eventLock, IDIssuer <Track> *trackIDIssuer, CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer,
		int trackAmount, int phraseAmount, int midiEventAmount);
	
	// GUI Stuff
	void createNewTrack();

	

	void workerCreateNewTrack();
	void static workerCreateNewTrack_(SongModel *model);

	void workerCreateNewPhrase(ID <Track> trackID);
	void static workerCreateNewPhrase(SongModel *songModel, ID <Track> trackID);

	// songModelLock must be acquired!!
	Pool <Track *> *getTracks();

private:
	CASLock *songModelLock_, *eventLock_;
	IDIssuer <Track> *trackIDIssuer_;
	CallbackConcreteRegisterer <SongMessage> *songMessageRegisterer_;

	// Slow stuff !!
	// songModelLock must be acquired!!

	Pool <Track *> tracks_;

	Pool <Track *> trackPool_;
	Pool <Phrase *> phrasePool_;
	Pool <MidiEvent *> midiEventPool_;

};

#endif
