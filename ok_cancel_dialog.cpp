#include "ok_cancel_dialog.h"

OkCancelDialog::OkCancelDialog(WindowSystem *windowSystem, std::string message, std::string okName, std::string cancelName) :
windowSystem_(windowSystem), message_(message), okName_(okName), cancelName_(cancelName)
{
	setOkCancelDialog();
	update();
}

void OkCancelDialog::setOkCancelDialog()
{
	okCancelDialogWindow_.addRegion("MessageLabel",
		10, ATTACH_NONE, "",
		10, ATTACH_NONE, "",
		300, ATTACH_NONE, "",
		20, ATTACH_NONE, "");

	okCancelDialogWindow_.addRegion("OkButton",
		0, ATTACH_LEFT, "MessageLabel",
		10, ATTACH_BOTTOM, "MessageLabel",
		60, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	okCancelDialogWindow_.addRegion("CancelButton",
		10, ATTACH_RIGHT, "OkButton",
		0, ATTACH_TOP, "OkButton",
		60, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	okCancelDialogWindow_.setWidget("MessageLabel", &messageLabel_);
	okCancelDialogWindow_.setWidget("OkButton", &okButton_);
	okCancelDialogWindow_.setWidget("CancelButton", &cancelButton_);
	
}

void OkCancelDialog::openOkCancelDialog()
{
	int x, y;

	windowSystem_->getTopWindowCoordinates(&x, &y);

	okCancelDialogWindow_.setX(x + 20);
	okCancelDialogWindow_.setY(y + 20);

	okCancelDialogWindow_.setWidth(300);
	okCancelDialogWindow_.setHeight(200);

	windowSystem_->addWindow(&okCancelDialogWindow_);
}

void OkCancelDialog::closeOkCancelDialog()
{
	windowSystem_->closeWindow(&okCancelDialogWindow_);
}


void OkCancelDialog::update()
{
	messageLabel_.text_ = message_;
	okButton_.setName(okName_);
	cancelButton_.setName(cancelName_);
}

void OkCancelDialog::setMessage(std::string message)
{
	message_ = message;

	update();
}


void OkCancelDialog::setOkName(std::string okName)
{
	okName_ = okName;

	update();
}

void OkCancelDialog::setCancelName(std::string cancelName)
{
	cancelName_ = cancelName;

	update();
}