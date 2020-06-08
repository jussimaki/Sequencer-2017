#ifndef FILE_WINDOW_CONTROLLER_H
#define FILE_WINDOW_CONTROLLER_H

#include "window.h"

class FileWindowModel;
class FileWindowView;



class FileWindowController
{


public:
	
	FileWindowController(FileWindowModel *model, FileWindowView *view);
	void configure();

	void static fileWindowOpenButtonCallback_(void *object, MouseEvent *event)
	{
		((FileWindowController*)object)->fileWindowOpenButtonCallback(event);
	}

	void static fileWindowBackButtonCallback_(void *object, MouseEvent *event)
	{
		((FileWindowController*)object)->fileWindowBackButtonCallback(event);
	}

	void static fileWindowCancelButtonCallback_(void *object, MouseEvent *event)
	{
		((FileWindowController*)object)->fileWindowCancelButtonCallback(event);
	}

	void static fileWindowFileListCallback_(void *object, ChooseEvent *event)
	{
		((FileWindowController*)object)->fileWindowFileListCallback(event);
	}

	void fileWindowOpenButtonCallback(MouseEvent *event);
	void fileWindowBackButtonCallback(MouseEvent *event);
	void fileWindowCancelButtonCallback(MouseEvent *event);
	void fileWindowFileListCallback(ChooseEvent *event);

	


	void openFileWindow();



private:
	FileWindowView *view_;
	FileWindowModel *model_;

	//std::string openedFileName_;

	// callback pointers for file opening
	//std::vector <void(*)(void *, FileWindowController*)> functionPointers_;
	//std::vector <void *> objects_;

	FileWindowController();

};

#endif