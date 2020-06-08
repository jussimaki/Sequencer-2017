#include "instrument_settings_view.h"
#include "instrument_settings_model.h"

#include "lock_guard.h"
#include "lock_levels.h"

extern thread_local LockHierarchy localLockHierarchy;


void InstrumentSettingsView::workerRefreshFromModel()
{
	std::vector<std::string> *sampleDataNameList = new std::vector<std::string>;
	std::vector<std::string> *sampleNameList = new std::vector<std::string>;
	std::vector<std::string> *instrumentNameList = new std::vector<std::string>;
	

	{
		LockGuard viewGuard(&localLockHierarchy, &instrumentSettingsViewLock_, 1, LOCK_LEVELS::GUI);
		{
			LockGuard modelGuard(&localLockHierarchy, &instrumentSettingsModelLock_, 1, LOCK_LEVELS::PROGRAM_LOGIC);

			sampleDataSample_ = instrumentSettingsModel_->getSampleDataSample();

			*sampleDataNameList = instrumentSettingsModel_->getSampleDataNameList();
			*sampleNameList = instrumentSettingsModel_->getSampleNameList();
			*instrumentNameList = instrumentSettingsModel_->getInstrumentNameList();
		}

		drawWaveFormToCanvas();

		InstrumentSettingsViewInternalMessage *message = new InstrumentSettingsViewInternalMessage;
		message->type_ = InstrumentSettingsViewInternalMessage::MessageType::DATA_REFRESHED;
		
		LockGuard eventGuard(&localLockHierarchy, &eventLock_, 1, LOCK_LEVELS::EVENTS);
		instrumentSettingsViewInternalRegisterer_->notify(message);
	}

	sampleList_.setNextElements(sampleDataNameList);
	loadedSamplesList_.setNextElements(sampleNameList);
	instrumentList_.setNextElements(instrumentNameList);
}

void InstrumentSettingsView::workerRefreshFromModel_(InstrumentSettingsView *view)
{
	view->workerRefreshFromModel();
}

// instrumentSettingsViewLock_ must be acquired to call this function!!
void InstrumentSettingsView::drawWaveFormToCanvas()
{
	sampleCanvasUpdated_ = true;

	for (unsigned int index = 0; index < waveFormCanvas_.width*waveFormCanvas_.height; index++)
	{
		waveFormCanvas_.data[index * 4] = 0;
		waveFormCanvas_.data[index * 4 + 1] = 0;
		waveFormCanvas_.data[index * 4 + 2] = 0;
		waveFormCanvas_.data[index * 4 + 3] = 255;
	}


	signed short *data = sampleDataSample_.getData();

	if (!data) return;

	unsigned int length = sampleDataSample_.getLength();
	signed short lastValue = 0;

	for (unsigned int x = 0; x < waveFormCanvas_.width; x++)
	{
		unsigned int sampleIndex = (int)((double)(length - 1)*(double)x / (double)waveFormCanvas_.width);

		if (sampleIndex < length)
		{

			signed short value = data[sampleIndex];

			signed short smaller, bigger;

			if (value < lastValue)
			{
				smaller = value;
				bigger = lastValue;
			}
			else
			{
				smaller = lastValue;
				bigger = value;
			}

			unsigned int topY = (waveFormCanvas_.height / 2) - (int)((double)(bigger)*(double)(waveFormCanvas_.height / 2) / (double)32768);
			unsigned int bottomY = (waveFormCanvas_.height / 2) - (int)((double)(smaller)*(double)(waveFormCanvas_.height / 2) / (double)32768);
			if (topY > waveFormCanvas_.height - 1) topY = waveFormCanvas_.height - 1;
			if (bottomY > waveFormCanvas_.height - 1) bottomY = waveFormCanvas_.height - 1;

			for (unsigned int y = topY; y <= bottomY; y++)
			{
				waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4)] = 255;
				waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 1] = 0;
				waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 2] = 0;
				waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 3] = 255;
			}

			unsigned int y = (int)((double)x / (double)(waveFormCanvas_.width - 1)*(double)(waveFormCanvas_.height - 1));

			waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4)] = 255;
			waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 1] = 255;
			waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 2] = 255;
			waveFormCanvas_.data[((y*waveFormCanvas_.width + x) * 4) + 3] = 255;


			lastValue = value;
		}
	}
}