#include "sample.h"


void Sample::copyData(signed short *data, unsigned int length)
{
	release();

	data_ = new signed short[length];
	
	memcpy((void*)data_, (void*)data, length * sizeof(signed short));
	
	length_ = length;
}

void Sample::release()
{
	if (data_)
	{
		delete[] data_;
		data_ = 0;
		length_ = 0;
	}
}