#ifndef WRAPPER_H
#define WRAPPER_H

#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <fstream>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#error NOT_WINDOWS
#endif

#include "keysymbols.h"





struct BMPData
{
	unsigned int width, height;
	unsigned int orig_width, orig_height;
	unsigned char *data;
};



struct Sprite
{
	unsigned int width, height;
	unsigned int orig_width, orig_height;

	unsigned char *data;
	GLuint openGLtexture;
};

struct Canvas
{
	Sprite *sprite;
	unsigned int width, height;
	unsigned char *data;
};


struct Rect
{
	int x, y, width, height;
};



struct InputData
{
	int mouse_move_x, mouse_move_y;

	Uint8 KEY_DOWN[KEY_LAST];

	unsigned char PAD_BUTTON_DOWN[24];

	unsigned char MOUSE_BUTTON_DOWN[3];
};



class Wrapper
{
private:
	Sprite *lastSprite_;
	int xResolution_, yResolution_;
	SDL_Surface *SDLScreen_;


	InputData inputData_;

	void(*soundSystemFunctionPointer_)(void *, unsigned char*, int);
	void *soundSystemObject_;
public:
	Wrapper() : lastSprite_(0), xResolution_(0), yResolution_(0), SDLScreen_(0), soundSystemFunctionPointer_(0), soundSystemObject_(0)
	{


	}

	~Wrapper()
	{
		std::cout << "~Wrapper()";
		shutDown();
	}

	void setSoundSystem(void(*soundSystemFunctionPointer)(void*, unsigned char*, int), void *soundSystemObject)
	{
		soundSystemFunctionPointer_ = soundSystemFunctionPointer;
		soundSystemObject_ = soundSystemObject;
	}


	void shutDownSound()
	{
		SDL_CloseAudio();
	}



	bool initialize(int x_resolution, int y_resolution, unsigned int sampleRate);
	bool shutDown();

	void getCanvas(Canvas *canvas, Sprite *sprite);
	void releaseCanvas(Canvas *canvas);
	void attachCanvas(Canvas *canvas);

	bool emptySprite(Sprite *sprite, unsigned int width, unsigned int height);
	bool loadSprite(Sprite *sprite, char *fileName);
	void releaseSprite(Sprite *sprite);

	bool drawSprite(Sprite *sprite, Rect *dest, Rect *source, Rect *clip, bool colorkey, int alpha);
	void drawRectangle(Rect *dest, Rect *clip, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha);

	BMPData* readBMPData(char *fileName);
	void reverseBMPData(BMPData *data);

	void clearScreen();
	void flip();

	int getXResolution();
	int getYResolution();


	InputData *getInputData();
	bool readInput();


	//bool doesCodeEqualString(char *string, unsigned int code);



	// audio stuff

	void audioCallback(Uint8 *stream, int len);
	void static audioCallback_(void *wrapperObject, Uint8 *stream, int len);




};

#endif