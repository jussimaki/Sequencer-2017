#include "file_window_model.h"
#include "spawn_thread.h"


FileWindowModel::FileWindowModel(CASLock *fileWindowModelLock, CASLock *eventLock, CASLock *fileLock,
	CallbackConcreteRegisterer <FileWindowMessage> *fileWindowMessageRegisterer,
	CallbackConcreteRegisterer<FileWindowModelInternalMessage> *fileWindowModelInternalRegisterer) :
	open_(false),
	objectSelected_(false), selectedObjectNumber_(0),
	fileWindowModelLock_(fileWindowModelLock),
	eventLock_(eventLock),
	fileLock_(fileLock),
	fileWindowMessageRegisterer_(fileWindowMessageRegisterer),
	fileWindowModelInternalRegisterer_(fileWindowModelInternalRegisterer)
{
	fileWindowModelInternalRegisterer_->registerListener(this, internalCallback_);
}

FileWindowModel::~FileWindowModel()
{
	fileWindowModelInternalRegisterer_->unregisterListener(this);
}

void FileWindowModel::registerObserver(Observer *observer)
{
	subject_.registerObserver(observer);
}

void FileWindowModel::unregisterObserver(Observer *observer)
{
	subject_.unregisterObserver(observer);
}

bool FileWindowModel::internalCallback(FileWindowModelInternalMessage *message)
{
	switch (message->type_)
	{
	case FileWindowModelInternalMessage::MessageType::FILE_OBJECTS_READ:

		objectSelected_ = false;
		selectedObjectNumber_ = 0;
		subject_.notify();
		break;

	case FileWindowModelInternalMessage::MessageType::FILE_CHOSEN:
		open_ = false;
		subject_.notify();
		break;

	default:
		break;
	}
	
	return true;
}


bool FileWindowModel::internalCallback_(void *model, FileWindowModelInternalMessage *message)
{
	return ((FileWindowModel*)model)->internalCallback(message);
}

void FileWindowModel::open()
{
	open_ = true;
}

bool FileWindowModel::isOpen()
{
	return open_;
}

bool FileWindowModel::isObjectSelected()
{
	return objectSelected_;
}

unsigned int FileWindowModel::getSelectedObjectNumber()
{
	return selectedObjectNumber_;
}

void FileWindowModel::selectObject(unsigned int number)
{
	objectSelected_ = true;
	selectedObjectNumber_ = number;

	subject_.notify();
}


void FileWindowModel::advance()
{
	if (objectSelected_)
	{
		COUNTED_THREAD worker(workerAdvance_, this, selectedObjectNumber_);

	}
}

void FileWindowModel::goBack()
{
	COUNTED_THREAD worker(workerGoBack_, this);

}


void FileWindowModel::initData()
{
	COUNTED_THREAD worker(workerInitData_, this);

}