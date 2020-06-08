#include "main_controller.h"

#include "instrument_settings_view.h"
//#include "instrument_settings_controller.h"

MainController::MainController(Sequencer *sequencer, bool *quitSignal, MainView *mainView,  InstrumentSettingsView *instrumentSettingsView) :
sequencer_(sequencer), quitSignal_(quitSignal), mainView_(mainView), instrumentSettingsView_(instrumentSettingsView)
{
	setMainController();
}

void MainController::setMainController()
{
	mainView_->setNewButtonCallback(this, &newButtonClicked_);
	mainView_->setSaveButtonCallback(this, &saveButtonClicked_);
	mainView_->setLoadButtonCallback(this, &loadButtonClicked_);
	mainView_->setQuitButtonCallback(this, &quitButtonClicked_);

	mainView_->setInstrumentsButtonCallback(this, &instrumentsButtonClicked_);
	mainView_->setSamplesButtonCallback(this, &saveButtonClicked_);
	mainView_->setTracksButtonCallback(this, &tracksButtonClicked_);
}

void MainController::instrumentsButtonClicked(MouseEvent *event)
{
	instrumentSettingsView_->openInstrumentSettingsView();
}

void MainController::tracksButtonClicked(MouseEvent *event)
{
	//trackSettingsView->openTrackSettingsView();
}

void MainController::newButtonClicked(MouseEvent *event)
{

}

void MainController::quitButtonClicked(MouseEvent *event)
{
	*quitSignal_ = true;
}


void MainController::loadButtonClicked(MouseEvent *event)
{
	//fileWindowView->openFileWindowView();
}

void MainController::saveButtonClicked(MouseEvent *event)
{

}