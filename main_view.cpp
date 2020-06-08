#include "main_view.h"
#include "instrument_sample_model.h"

#include "tracks_edit_view.h"

MainView::MainView(InstrumentSampleModel *instrumentSampleModel, WindowSystem *windowSystem, TracksEditView *tracksEditView) : instrumentSampleModel_(instrumentSampleModel), windowSystem_(windowSystem), tracksEditView_(tracksEditView)
{
	setMainView();


	//instrumentSampleModel_->registerObserver(this);

	update(0);
}

MainView::~MainView()
{
	//instrumentSampleModel_->getSubject()->unregisterObserver(this);
}

void MainView::setMainView()
{
	mainWindow_.addRegion("NewButton",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		100, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("SaveButton",
		10, ATTACH_RIGHT, "NewButton",
		0, ATTACH_TOP, "NewButton",
		100, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("LoadButton",
		10, ATTACH_RIGHT, "SaveButton",
		0, ATTACH_TOP, "SaveButton",
		100, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("QuitButton",
		10, ATTACH_RIGHT, "LoadButton",
		0, ATTACH_TOP, "LoadButton",
		100, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("InstrumentsButton",
		300, ATTACH_RIGHT, "QuitButton",
		0, ATTACH_TOP, "QuitButton",
		150, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("SamplesButton",
		10, ATTACH_RIGHT, "InstrumentsButton",
		0, ATTACH_TOP, "InstrumentsButton",
		150, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("TracksButton",
		10, ATTACH_RIGHT, "SamplesButton",
		0, ATTACH_TOP, "SamplesButton",
		150, ATTACH_NONE, "",
		70, ATTACH_NONE, "");

	mainWindow_.addRegion("TracksViewRegion",
		0, ATTACH_LEFT, "NewButton",
		10, ATTACH_BOTTOM, "NewButton",
		1600, ATTACH_NONE, "",
		700, ATTACH_NONE, "");


	mainWindow_.setWidget("NewButton", &newButton_);
	mainWindow_.setWidget("SaveButton", &saveButton_);
	mainWindow_.setWidget("LoadButton", &loadButton_);
	mainWindow_.setWidget("QuitButton", &quitButton_);

	mainWindow_.setWidget("InstrumentsButton", &instrumentsButton_);
	mainWindow_.setWidget("SamplesButton", &samplesButton_);
	mainWindow_.setWidget("TracksButton", &tracksButton_);

	mainWindow_.setWidget("TracksViewRegion", tracksEditView_->getTracksEditWindow());


	//mainWindow_.setWidget("TracksViewRegion", &tracksButton_);

	newButton_.setName("New song");
	loadButton_.setName("Load song");
	saveButton_.setName("Save song");
	quitButton_.setName("Quit");

	instrumentsButton_.setName("Instrument settings");
	samplesButton_.setName("Samplebank");
	tracksButton_.setName("Track settings");

	


}

void MainView::openMainView()
{
	mainWindow_.setX(0);
	mainWindow_.setY(0);

	mainWindow_.setWidth(windowSystem_->getWidth());
	mainWindow_.setHeight(windowSystem_->getHeight());
	windowSystem_->addWindow(&mainWindow_);

}

void MainView::update(Subject *subject)
{

}