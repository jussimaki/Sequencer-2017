#include "file_window_model.h"
#include "file_window_view.h"
#include "file_window_controller.h"


FileWindowController::FileWindowController(FileWindowModel *model, FileWindowView *view) : model_(model), view_(view)
{
	configure();
}

void FileWindowController::openFileWindow()
{
	view_->open();
}

void FileWindowController::configure()
{
	view_->setFileOpenButtonCallback(this, &(FileWindowController::fileWindowOpenButtonCallback_));
	view_->setFileBackButtonCallback(this, &(FileWindowController::fileWindowBackButtonCallback_));
	view_->setFileCancelButtonCallback(this, &(FileWindowController::fileWindowCancelButtonCallback_));

	view_->setFileListCallback(this, &(FileWindowController::fileWindowFileListCallback_));
}


void FileWindowController::fileWindowOpenButtonCallback(MouseEvent *event)
{
	model_->advance();
	
}

void FileWindowController::fileWindowBackButtonCallback(MouseEvent *event)
{
	model_->goBack();
}

void FileWindowController::fileWindowCancelButtonCallback(MouseEvent *event)
{
	view_->close();
}

void FileWindowController::fileWindowFileListCallback(ChooseEvent *event)
{
	model_->selectObject(event->getNumber());
}