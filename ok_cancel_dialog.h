#ifndef OK_CANCEL_DIALOG_H
#define OK_CANCEL_DIALOG_H

#include "window.h"


class OkCancelDialog
{
public:
	OkCancelDialog(WindowSystem *windowSystem, std::string message, std::string okName, std::string cancelName);

	void setOkCancelDialog();
	void openOkCancelDialog();
	void closeOkCancelDialog();
	void update();

	void setMessage(std::string message);
	void setOkName(std::string okName);
	void setCancelName(std::string cancelName);

	void setOkButtonEnabled(bool value)
	{
		if (value)
		{
			okButton_.activate();
		}
		else
		{
			okButton_.deactivate();
		}
	}
	

	void setOkButtonCallback(void *object, void (*functionPointer)(void *, MouseEvent *event))
	{
		okButton_.onClick_.add(object, functionPointer);
	}

	void setCancelButtonCallback(void *object, void (*functionPointer)(void *, MouseEvent *event))
	{
		cancelButton_.onClick_.add(object, functionPointer);
	}

private:
	WindowSystem *windowSystem_;
	std::string message_;
	std::string okName_;
	std::string cancelName_;


	Window okCancelDialogWindow_;

	Label messageLabel_;
	Button okButton_;
	Button cancelButton_;
};

#endif