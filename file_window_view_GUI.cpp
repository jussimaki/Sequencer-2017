#include "file_window_view.h"
#include "file_window_model.h"

#include "lock_hierarchy.h"
#include "lock_levels.h"
#include "lock_guard.h"
#include "spawn_thread.h"

extern thread_local LockHierarchy localLockHierarchy;


FileWindowView::FileWindowView(FileWindowModel *model, CallbackConcreteRegisterer <FileWindowViewInternalMessage> *fileWindowViewInternalRegisterer,
	CASLock *fileWindowModelLock, CASLock *fileWindowViewLock, CASLock *eventLock, WindowSystem *system) :
	model_(model),
	fileWindowViewInternalRegisterer_(fileWindowViewInternalRegisterer),
	fileWindowModelLock_(fileWindowModelLock),
	fileWindowViewLock_(fileWindowViewLock),
	eventLock_(eventLock),
	system_(system)
{
	createLayout();
	configureLayout();

	fileWindowViewInternalRegisterer_->registerListener(this, &internalCallback_);

	model_->registerObserver(this);

	//update(0);
}

FileWindowView::~FileWindowView()
{
	model_->unregisterObserver(this);
}

bool FileWindowView::internalCallback(FileWindowViewInternalMessage *message)
{
	return updateGUI();
}
bool FileWindowView::internalCallback_(void *view, FileWindowViewInternalMessage *message)
{
	return ((FileWindowView*)view)->internalCallback(message);
}

void FileWindowView::createLayout()
{

	fileWindow_.addRegion("DirectoryLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		600, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileList",
		0, ATTACH_LEFT, "DirectoryLabel",
		10, ATTACH_BOTTOM, "DirectoryLabel",
		200, ATTACH_NONE, "",
		300, ATTACH_NONE, "");

	fileWindow_.addRegion("FileOpenButton",
		0, ATTACH_LEFT, "FileList",
		10, ATTACH_BOTTOM, "FileList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileBackButton",
		10, ATTACH_RIGHT, "FileOpenButton",
		0, ATTACH_TOP, "FileOpenButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	fileWindow_.addRegion("FileCancelButton",
		10, ATTACH_RIGHT, "FileBackButton",
		0, ATTACH_TOP, "FileBackButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");



}

void FileWindowView::configureLayout()
{
	fileWindow_.setWidget("DirectoryLabel", &directoryLabel_);
	fileWindow_.setWidget("FileList", &fileList_);
	fileWindow_.setWidget("FileOpenButton", &fileOpenButton_);
	fileWindow_.setWidget("FileBackButton", &fileBackButton_);
	fileWindow_.setWidget("FileCancelButton", &fileCancelButton_);

	fileList_.setVScrollControlsActive(true);

	fileOpenButton_.setName("Open");
	fileBackButton_.setName("Back");
	
	fileCancelButton_.setName("Cancel");
}

void FileWindowView::update(Subject *subject)
{
	COUNTED_THREAD thread(workerRefreshFromModel_, this);
	
	updateGUI();
}

bool FileWindowView::updateGUI()
{
	if (!model_->isOpen())
		close();

	bool error = false;

	if (localLockHierarchy.tryToAcquire(&fileWindowViewLock_, 1, LOCK_LEVELS::GUI))
	{
		if (model_->isObjectSelected())
			fileList_.setRow(selectedObjectNumber_);
		else
		{
			fileList_.setRow(0);
			fileList_.setSelected(false);
		}

		directoryLabel_.text_ = directoryName_;


		localLockHierarchy.release();
	}
	else error = true;


	return !error;
}

void FileWindowView::open()
{
	int x, y;
	system_->getTopWindowCoordinates(&x, &y);
	fileWindow_.setX(x + 30);
	fileWindow_.setY(y + 30);
	fileWindow_.setWidth(500);
	fileWindow_.setHeight(500);

	system_->addWindow(&fileWindow_);

	model_->initData();
	model_->open();
}

void FileWindowView::close()
{
	system_->closeWindow(&fileWindow_);
}