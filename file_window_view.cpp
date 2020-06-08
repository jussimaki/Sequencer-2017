#include "file_window_view.h"
#include "file_window_model.h"

#include "lock_guard.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;

void FileWindowView::workerRefreshFromModel()
{

	std::vector<std::string> *fileObjectNameList = new std::vector<std::string>;
	{
		LockGuard viewGuard(&localLockHierarchy, &fileWindowViewLock_, 1, LOCK_LEVELS::GUI);
		LockGuard modelGuard(&localLockHierarchy, &fileWindowModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

		std::vector<FileObject> fileObjectList = model_->getFileObjects();

		for (unsigned int index = 0; index < fileObjectList.size(); index++)
		{
			std::string name = fileObjectList[index].name;
			if (fileObjectList[index].directory) name += " (DIR)";
			else if (fileObjectList[index].drive) name += " (DRIVE)";

			fileObjectNameList->push_back(name);
		}


		directoryName_ = model_->getDirectoryName();
		objectSelected_ = model_->isObjectSelected();
		selectedObjectNumber_ = model_->getSelectedObjectNumber();

		FileWindowViewInternalMessage *message = new FileWindowViewInternalMessage;
		message->type_ = FileWindowViewInternalMessage::MessageType::DATA_REFRESHED;

		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
		fileWindowViewInternalRegisterer_->notify(message);
	}

	fileList_.setNextElements(fileObjectNameList);
}

void FileWindowView::workerRefreshFromModel_(FileWindowView *view)
{
	view->workerRefreshFromModel();
}







