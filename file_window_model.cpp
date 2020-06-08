#include "file_window_model.h"

#include "lock_hierarchy.h"
#include "lock_guard.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;



void FileWindowModel::workerAdvance(unsigned int objectNumber)
{

	LockGuard modelGuard(&localLockHierarchy, &fileWindowModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

	if (objectNumber >= fileObjects_.size()) return;
	
	FileObject object = fileObjects_[objectNumber];

	// selected object isn't a file
	if (object.directory || object.drive)
	{
		// advance in the file system
		{
			LockGuard fileGuard(&localLockHierarchy, &fileLock_, 1, LOCK_LEVELS::IO);

			FILESYSTEM_advance(&directory_, &object);

			fileObjects_.clear();

			FILESYSTEM_getFileObjects(&directory_, &fileObjects_);
			directoryName_ = FILESYSTEM_getFileNameString(&directory_);
		}

		// send only internal messages
		FileWindowModelInternalMessage *message = new FileWindowModelInternalMessage;
		message->type_ = FileWindowModelInternalMessage::MessageType::FILE_OBJECTS_READ;

		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
		fileWindowModelInternalRegisterer_->notify(message);
	}
	else
	{
		{
			LockGuard fileGuard(&localLockHierarchy, &fileLock_, 1, LOCK_LEVELS::IO);
			advanceToFile(&object);
		}

		// send an internal message
		FileWindowModelInternalMessage *internalMessage = new FileWindowModelInternalMessage;
		internalMessage->type_ = FileWindowModelInternalMessage::MessageType::FILE_CHOSEN;

		// also send a message to those that are interested about the file being chosen
		FileWindowMessage *message = new FileWindowMessage;
		message->type_ = FileWindowMessage::MessageType::FILE_CHOSEN;
		message->fileName_ = finalFileName_;

		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);

		fileWindowModelInternalRegisterer_->notify(internalMessage);
		fileWindowMessageRegisterer_->notify(message);
	}

}

void FileWindowModel::workerAdvance_(FileWindowModel *fileWindowModel, unsigned int objectNumber)
{
	fileWindowModel->workerAdvance(objectNumber);
}


void FileWindowModel::workerGoBack()
{
	LockGuard modelGuard(&localLockHierarchy, &fileWindowModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

	{
		LockGuard fileGuard(&localLockHierarchy, &fileLock_, 1, LOCK_LEVELS::IO);

		FILESYSTEM_goBack(&directory_);

		fileObjects_.clear();

		FILESYSTEM_getFileObjects(&directory_, &fileObjects_);

		directoryName_ = FILESYSTEM_getFileNameString(&directory_);
	}


	FileWindowModelInternalMessage *message = new FileWindowModelInternalMessage;
	message->type_ = FileWindowModelInternalMessage::MessageType::FILE_OBJECTS_READ;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	fileWindowModelInternalRegisterer_->notify(message);
	
}

void FileWindowModel::workerGoBack_(FileWindowModel *fileWindowModel)
{
	fileWindowModel->workerGoBack();
}

void FileWindowModel::workerInitData()
{
	LockGuard modelGuard(&localLockHierarchy, &fileWindowModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

	{
		LockGuard fileGuard(&localLockHierarchy, &fileLock_, 1, LOCK_LEVELS::IO);

		FILESYSTEM_getCurrentDirectory(&directory_);

		fileObjects_.clear();

		FILESYSTEM_getFileObjects(&directory_, &fileObjects_);

		directoryName_ = FILESYSTEM_getFileNameString(&directory_);
	}

	FileWindowModelInternalMessage *message = new FileWindowModelInternalMessage;
	message->type_ = FileWindowModelInternalMessage::MessageType::FILE_OBJECTS_READ;

	LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
	fileWindowModelInternalRegisterer_->notify(message);
}

void FileWindowModel::workerInitData_(FileWindowModel *fileWindowModel)
{
	fileWindowModel->workerInitData();
}


std::vector <FileObject> FileWindowModel::getFileObjects()
{
	return fileObjects_;
}


// NOTE: fileWindowModelLock_ and fileLock_ must be acquired!!
void FileWindowModel::advanceToFile(FileObject *object)
{
	FILESYSTEM_copyDirectoryData(&directory_, &fileName_);
	FILESYSTEM_advance(&fileName_, object);

	finalFileName_ = FILESYSTEM_getFileNameString(&fileName_);
	
}

