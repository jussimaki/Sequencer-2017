#include "wave.h"
#include "file.h"

Wave::Wave(char *filename) : buffer_(0), channels_(0), sampleRate_(0), samplesPerChannel_(0), loaded_(false)
{
	load(filename);
}

Wave::~Wave()
{
	release();
}

void Wave::release()
{
	if (loaded_)
	{
		if (buffer_)
		{
			delete[] buffer_;
			buffer_ = 0;
		}

		channels_ = 0;
		sampleRate_ = 0;
		samplesPerChannel_ = 0;

		loaded_ = false;
	}
}

// loads a 16 bit mono .wav file
bool Wave::load(char *fileName)
{
	release();

	BinaryFile file;

	openBinaryFile(fileName, &file, LITTLE_ENDIAN);

	if (!file.file.is_open()) return false;

	unsigned int RIFF = readUDword(&file);

	if (RIFF != 0x46464952) // "RIFF"
	{
		closeBinaryFile(&file); return false;
	}

	unsigned int chunkSize = readUDword(&file);

	unsigned int WAVE = readUDword(&file);

	if (WAVE != 0x45564157) // "WAVE"
	{
		closeBinaryFile(&file); return false;
	}


	unsigned int subchunkID;
	unsigned int subchunkSize;
	bool done = false;

	// "fmt "

	short int format;
	short int channels;
	unsigned int sampleRate;
	unsigned short bitsPerSample;



	while (!done)
	{
		unsigned int bytesRead = 0;

		subchunkID = readUDword(&file);
		subchunkSize = readUDword(&file);

		// read subchunk
		switch (subchunkID)
		{
		case 0x20746D66: // "fmt "

			format = readUWord(&file);

			if (format != 1) // must be PCM (1)
			{
				closeBinaryFile(&file); return false;
			}

			channels = readUWord(&file);
			channels_ = channels;

			sampleRate = readUDword(&file);
			sampleRate_ = sampleRate;

			readUDword(&file);
			readUWord(&file);

			bitsPerSample = readUWord(&file);

			if (bitsPerSample != 16)
			{
				closeBinaryFile(&file); return false;
			}


			bytesRead = 16;
			break;


		case 0x61746164:	// "data"


			samplesPerChannel_ = ((subchunkSize) / (bitsPerSample / 8)) / channels_;
			buffer_ = new signed short[(subchunkSize) / (bitsPerSample / 8)];
			readToBuffer((char*)buffer_, sizeof(signed short)*((subchunkSize) / (bitsPerSample / 8)), &file);
			//readToBuffer((char*)buffer_, subchunkSize, &file);

			bytesRead = subchunkSize;
			done = true;

			break;

		default:
			bytesRead = 0;
			break;
		}


		if (!done) for (unsigned int index = 0; index < subchunkSize - bytesRead; index++) readByte(&file);

	}

	closeBinaryFile(&file);

	loaded_ = true;

	return true;
}