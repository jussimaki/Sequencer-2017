#ifndef WAVE_H
#define WAVE_H


class Wave
{
private:
	signed short *buffer_;
	short channels_;
	unsigned int sampleRate_;

	unsigned int samplesPerChannel_;

	bool loaded_;
public:
	Wave() : buffer_(0), channels_(0), sampleRate_(0), samplesPerChannel_(0), loaded_(false) {}
	Wave(char *fileName);
	~Wave();


	bool load(char *filename);
	void release();
	void unLoad() { loaded_ = false; buffer_ = 0; }

	signed short *getBuffer() { return buffer_; }
	short getChannels() { return channels_; }
	short getSampleRate() { return sampleRate_; }
	unsigned int getSamplesPerChannel() { return samplesPerChannel_; }
		
	
};

#endif