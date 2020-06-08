#ifndef FILE_WINDOW_VIEW_H
#define FILE_WINDOW_VIEW_H

#include "window.h"
#include "window_system.h"
#include "observer.h"
#include "callback_registerer.h"

class FileWindowModel;
class CASLock;
class FileWindowViewInternalMessage
{
public:
	enum MessageType { DATA_REFRESHED };

	MessageType type_;
};


class FileWindowView : public Observer
{
	
public:


	void setFileOpenButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		fileOpenButton_.onClick_.add(object, functionPointer);
	}

	void setFileBackButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		fileBackButton_.onClick_.add(object, functionPointer);
	}

	void setFileCancelButtonCallback(void *object, void(*functionPointer)(void*, MouseEvent*))
	{
		fileCancelButton_.onClick_.add(object, functionPointer);
	}

	void setFileListCallback(void *object, void(*functionPointer)(void*, ChooseEvent*))
	{
		fileList_.chooseEvent_.add(object, functionPointer);
	}

	FileWindowView(FileWindowModel *model, CallbackConcreteRegisterer<FileWindowViewInternalMessage> *fileWindowViewInternalRegisterer, CASLock *fileWindowModelLock,
		CASLock *fileWindowViewLock, CASLock *eventLock, WindowSystem *system);
	~FileWindowView();

	// GUI thread
	bool internalCallback(FileWindowViewInternalMessage *message);
	bool static internalCallback_(void *model, FileWindowViewInternalMessage *message);

	void createLayout();
	void configureLayout();
	
	void update(Subject *subject);
	bool updateGUI();

	void open();
	void close();

	// workers
	void workerRefreshFromModel();
	void static workerRefreshFromModel_(FileWindowView *view);



private:


	// GUI thread stuff
	Window fileWindow_;

	Label directoryLabel_;
	List fileList_;
	Button fileOpenButton_, fileBackButton_, fileCancelButton_;

	// Worker thread stuff
	// protected under fileWindowViewLock_
	std::string directoryName_;
	bool objectSelected_;
	unsigned int selectedObjectNumber_;


	FileWindowModel *model_;
	CallbackConcreteRegisterer <FileWindowViewInternalMessage> *fileWindowViewInternalRegisterer_;
	
	CASLock *fileWindowModelLock_;
	CASLock *fileWindowViewLock_;
	CASLock *eventLock_;

	WindowSystem *system_;


};


#endif