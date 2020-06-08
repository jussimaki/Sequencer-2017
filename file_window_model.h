#ifndef FILE_WINDOW_MODEL_H
#define FILE_WINDOW_MODEL_H

#include "file_functions.h"
#include "subject.h"
#include "callback_registerer.h"

class EventNotifier;
class CASLock;

class FileWindowModelInternalMessage
{
public:
	enum MessageType{FILE_OBJECTS_READ, FILE_CHOSEN};
	MessageType type_;
};

class FileWindowMessage
{
public:
	enum MessageType {FILE_CHOSEN};

	MessageType type_;
	std::string fileName_;
};

class FileWindowModel
{

public:
	FileWindowModel(CASLock *fileWindowModelLock, CASLock *eventLLock, CASLock *fileLock,
		CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer,
		CallbackConcreteRegisterer<FileWindowModelInternalMessage> *fileWindowModelInternalRegisterer);

	~FileWindowModel();

	void registerObserver(Observer *observer);
	void unregisterObserver(Observer *observer);

	// GUI thread callbacks
	bool internalCallback(FileWindowModelInternalMessage *message);
	bool static internalCallback_(void *model, FileWindowModelInternalMessage *message);


	// GUI thread functions
	// called by controller and view
	// (so called interface functions?)
	void open();
	bool isOpen();
	bool isObjectSelected();
	unsigned int getSelectedObjectNumber();
	void selectObject(unsigned int number);
	void advance();
	void goBack();
	void initData();


	// workers that are started in the interface functions
	void workerAdvance(unsigned int objectNumber);
	void static workerAdvance_(FileWindowModel *fileWindowModel, unsigned int objectNumber);
	void workerGoBack();
	void static workerGoBack_(FileWindowModel *fileWindowModel);
	void workerInitData();
	void static workerInitData_(FileWindowModel *fileWindowModel);


	//THREAD_FUNCTION(FileWindowModel, workerDiipaDaapa, int arvo);


	// Data interface for outside workers.
	// These access the slow data.
	// NOTE: fileWindowModelLock_ must be acquired to call
	// the following functions!!
	std::vector <FileObject> getFileObjects();
	std::string getDirectoryName() { return directoryName_; }
	std::string getFileName() { return finalFileName_; }

private:

	// helper functions that are called by workers
	// NOTE: fileWindowModelLock_ must be acquired to call
	// the following functions!!
	void advanceToFile(FileObject *object);
	CASLock *fileWindowModelLock_, *eventLock_, *fileLock_;

	CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer_;
	CallbackConcreteRegisterer<FileWindowModelInternalMessage> *fileWindowModelInternalRegisterer_;

	
	// slow data protected by fileWindowModelLock_
	// and manipulated in worker threads
	std::vector <FileObject> fileObjects_;
	DirectoryData directory_, fileName_;
	std::string finalFileName_;
	std::string directoryName_;


	// data manipulated straight in GUI thread,
	// no locks involved.
	bool open_;
	bool objectSelected_;
	unsigned int selectedObjectNumber_;
	
	Subject subject_;
};

#endif