#include "window_system.h"
#include "window.h"

#include <sstream>

extern int globalInt, globalInt2;

WindowSystem::~WindowSystem()
{
	windows_.clear();
	popups_.clear();

	for(unsigned int index = 0; index < spriteWindowSpritePairVector_.size(); index++)
	{
		wrapper_->releaseSprite(spriteWindowSpritePairVector_[index].sprite);

		if(spriteWindowSpritePairVector_[index].sprite) delete spriteWindowSpritePairVector_[index].sprite;
		spriteWindowSpritePairVector_[index].sprite = 0;
	}

	spriteWindowSpritePairVector_.clear();

	for (unsigned int index = 0; index < canvasWindowCanvasPairVector_.size(); index++)
	{
		wrapper_->releaseCanvas(&canvasWindowCanvasPairVector_[index].canvas);
	}

	canvasWindowCanvasPairVector_.clear();
}



Input *WindowSystem::getInput()
{
	return &input_;
}

void WindowSystem::setWrapper(Wrapper *wrapper)
{
	wrapper_ = wrapper;
}

Wrapper *WindowSystem::getWrapper()
{
	return wrapper_;
}


Printer *WindowSystem::getPrinter()
{
	return printer_;
}


void WindowSystem::addWindow(Window *window)
{
	window->setWindowSystem(this);
	windows_.push_back(window);
}

void WindowSystem::closeWindow(Window *window)
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		if(windows_[index] == window)
		{
			windows_.erase(windows_.begin() + index);

			return;
		}
	}
}

void WindowSystem::addPopup(Window *window)
{
	window->setWindowSystem(this);
	popups_.push_back(window);

	popupOpened_ = true;
}

void WindowSystem::closePopup(Window *window)
{
	for (unsigned int index = 0; index < popups_.size(); index++)
	{
		if (popups_[index] == window)
		{
			popups_.erase(popups_.begin() + index);

			return;
		}
	}
}

bool WindowSystem::popupOpened(Window *popup)
{
	for (int index = 0; index < popups_.size(); index++)
	{
		if (popups_[index] == popup) return true;
	}

	return false;
}

void WindowSystem::draw()
{
	Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.width = wrapper_->getXResolution();
	rect.height = wrapper_->getYResolution();

	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->update();

		windows_[index]->draw(windows_[index]->getX(), windows_[index]->getY(),
			windows_[index]->getWidth(), windows_[index]->getHeight(), rect);
	}

	for (unsigned int index = 0; index < popups_.size(); index++)
	{
		popups_[index]->draw(popups_[index]->getX(), popups_[index]->getY(),
			popups_[index]->getWidth(), popups_[index]->getHeight(), rect);
	}

	Rect dest;
	dest.x = mouseX_;
	dest.y = mouseY_;
	dest.width = 15;
	dest.height = 15;
	//printer_->print("M", mouseX_, mouseY_, 0);
	wrapper_->drawRectangle(&dest, 0, 255, 0, 255, 128);

	

	
}


bool WindowSystem::mouseOnPopup()
{

	bool mouseOnPopup = false;

	for (int index = 0; index < popups_.size(); index++)
	{
		int x = popups_[index]->getX();
		int y = popups_[index]->getY();

		int width = popups_[index]->getWidth();
		int height = popups_[index]->getHeight();

		if (mouseOnRectangle(x, y, width, height, mouseX_, mouseY_)) mouseOnPopup = true;

	}

	return mouseOnPopup;
}

void WindowSystem::logic()
{
	popupOpened_ = false;

	inputFunctionality(wrapper_, &input_);

	mouseX_ += input_.mouse_move_x;
	mouseY_ += input_.mouse_move_y;

	if (mouseX_ >= wrapper_->getXResolution()) mouseX_ = wrapper_->getXResolution() - 1;
	if (mouseX_ < 0) mouseX_ = 0;

	if (mouseY_ >= wrapper_->getYResolution()) mouseY_ = wrapper_->getYResolution() - 1;
	if (mouseY_ < 0) mouseY_ = 0;
	
	Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.width = wrapper_->getXResolution();
	rect.height = wrapper_->getYResolution();
	

	// calls the logic function for the top window
	// if there are no open popups
	int index = (int)windows_.size() - 1;
	if (windows_.size() > 0 && !mouseOnPopup())
		windows_[windows_.size() - 1]->logic(windows_[index]->getX(), windows_[index]->getY(), windows_[index]->getWidth(),
		windows_[index]->getHeight(), rect, mouseX_, mouseY_, &input_);

	for (int index = 0; index < popups_.size(); index++)
	{
		popups_[index]->logic(popups_[index]->getX(), popups_[index]->getY(), popups_[index]->getWidth(),
			popups_[index]->getHeight(), rect, mouseX_, mouseY_, &input_);
	}


	

	if (input_.MOUSE_BUTTON_CLICKED[0] && !popupOpened_)
	{
		if (!mouseOnPopup()) clearPopups();
	}
	
}

void WindowSystem::update()
{
	for(unsigned int index = 0; index < windows_.size(); index++)
	{
		windows_[index]->update();
	}
}

void WindowSystem::getTopWindowCoordinates(int *x, int *y)
{
	if (windows_.size() > 0)
	{
		*x = windows_[windows_.size() - 1]->getX();
		*y = windows_[windows_.size() - 1]->getY();
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}


WindowSprite WindowSystem::loadWindowSprite(char *fileName)
{
	SpriteWindowSpritePair pair;

	pair.sprite = new Sprite;

	wrapper_->loadSprite(pair.sprite, fileName);
		

	pair.windowSprite.handle = spriteCounter_;

	spriteCounter_++;

	pair.windowSprite.width = pair.sprite->orig_width;
	pair.windowSprite.height = pair.sprite->orig_height;

	spriteWindowSpritePairVector_.push_back(pair);

	return pair.windowSprite;
}

WindowSprite WindowSystem::getEmptyWindowSprite(unsigned int width, unsigned int height)
{
	SpriteWindowSpritePair pair;

	pair.sprite = new Sprite;

	wrapper_->emptySprite(pair.sprite, width, height);
	
	pair.windowSprite.handle = spriteCounter_;

	spriteCounter_++;


	pair.windowSprite.width = pair.sprite->orig_width;
	pair.windowSprite.height = pair.sprite->orig_height;

	spriteWindowSpritePairVector_.push_back(pair);
	
	return pair.windowSprite;
}

WindowCanvas WindowSystem::getWindowCanvas(WindowSprite windowSprite)
{
	
	Sprite *sprite = 0;
	// find the corresponding sprite for the windowSprite
	sprite = getSprite(windowSprite.handle);
	
	if (!sprite) MessageBeep(0);
		

	// get found sprite's canvas
	Canvas canvas;
	wrapper_->getCanvas(&canvas, sprite);


	
	

	WindowCanvas newWindowCanvas;
	newWindowCanvas.handle = canvasCounter_;
	canvasCounter_++;

	newWindowCanvas.width = windowSprite.width;
	newWindowCanvas.height = windowSprite.height;
	newWindowCanvas.data = canvas.data; // windowCanvas shares data with canvas
			

	// associate the sprite's canvas with windowCanvas's handle
	CanvasWindowCanvasPair pair;
	pair.windowCanvas = newWindowCanvas;
	pair.canvas = canvas;
	
	canvasWindowCanvasPairVector_.push_back(pair);

	return newWindowCanvas;
}


void WindowSystem::attachWindowCanvas(WindowCanvas windowCanvas)
{
	
	for (unsigned int index = 0; index < canvasWindowCanvasPairVector_.size(); index++)
	{
	
		if (canvasWindowCanvasPairVector_[index].windowCanvas.handle == windowCanvas.handle)
		{
			wrapper_->attachCanvas(&canvasWindowCanvasPairVector_[index].canvas);

			break;
		}
	}
	
}


// find the corresponding sprite for the windowSprite

Sprite *WindowSystem::getSprite(unsigned int handle)
{
	for (unsigned int index = 0; index < spriteWindowSpritePairVector_.size(); index++)
	{
		if (spriteWindowSpritePairVector_[index].windowSprite.handle == handle)
			return spriteWindowSpritePairVector_[index].sprite;
	}

	return 0;
}