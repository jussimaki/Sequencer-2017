// testi

#include <mutex>
#include <sstream>


#include "id.h"
#include "instrument_sample_model.h"
#include "instrument_settings_model.h"
#include "instrument_settings_view.h"
#include "instrument_settings_controller.h"
#include "callback_registerer.h"
#include "lock_hierarchy.h"
#include "cas_lock.h"
#include "lock_levels.h"
#include  "lock_guard.h"

#include "switchable.h"

#include "test2.h"


#include "wrapper.h"
#include "printer.h"
#include "window_system.h"

#include "song_model.h"
#include "tracks_edit_model.h"
#include "tracks_edit_view.h"
#include "tracks_edit_controller.h"
#include "main_view.h"
#include "main_controller.h"

#include "file_window_model.h"

#include "file_window_controller.h"
#include "thread_counter.h"

#undef main


int globalInt = 0, globalInt2 = 0, globalInt3 = 0;
std::string debugString;
LockHierarchy lockHierarchy(LOCK_LEVELS::END);


//std::atomic <int> threadCountX = 0;
//thread_local ThreadCounter threadCounter(&threadCountX);

std::atomic <int> threadCount = 0;
thread_local LockHierarchy localLockHierarchy(&lockHierarchy);

void test(CASLock *coutLockP, std::string string)
{
	while (1)
	{
		LockGuard coutGuard(&localLockHierarchy, &coutLockP, 1, LOCK_LEVELS::IO);
		//coutLockP->lock();
		std::cout << "function " << string << std::endl;
	}
}


bool blackHeightTest(RedBlackTree <int> *tree)
{
	
	bool counted = false;
	long lastHeight = 0;
	bool error = false;

	for (int index = 0; index < 1000000; index++)
	{
		RedBlackNode <int> *node = tree->find(index);

		if (node)
		{
			//if (!node->black) std::cout << "node " << index << " red" << std::endl;
			//else std::cout << "node " << index << " black" << std::endl;
			if (!node->leftChild && !node->rightChild)
			{
				long bHeight = tree->countBlackHeight(node);

				if (counted && bHeight != lastHeight)
				{
					error = true;

					std::cout << "HEIGHT ERROR at node " << index << std::endl;
					std::cout << lastHeight << ", " << bHeight << std::endl;

					return !error;
				
				}
				else  lastHeight = bHeight;

				//std::cout << "counted " << index << ", height = " << bHeight << std::endl;
				counted = true;
			}
		}

	}

	return (!error);
}

int main(int argc, char *argv[])
{

	

	/*
	RedBlackTree <int> tree;
	for (int index = 0; index < 1000000; index++)
	{
		RedBlackNode <int> *node = new RedBlackNode <int>;

		node->data = 666;

		node->value = index;

		tree.insert(node);
	}


	std::cout << "insert done" << std::endl;

	blackHeightTest(&tree);

	RedBlackNode <int> *error;
	if (tree.doubleRedError(tree.getRoot(), &error)) std::cout << "double red error at " << error->value << " !!" << std::endl;

	int iteration = 0;

	for (int ones = 0; ones < 10; ones++)
	{
		for (int tens = 0; tens < 10; tens++)
		{
			iteration++;

			std::cout << "iteration " << iteration << std::endl;

			for (int hundreds = 0; hundreds < 10; hundreds++)
			{
				for (int thousands = 0; thousands < 10; thousands++)
				{
					for (int tensofthousands = 0; tensofthousands < 10; tensofthousands++)
					{
						for (int hundredsofthousands = 0; hundredsofthousands < 10; hundredsofthousands++)
						{

							

							//std::cout << "remove " << hundredsofthousands*100000 + tensofthousands*10000 + thousands*1000 + hundreds*100 +  tens * 10 + ones << std::endl;
							RedBlackNode <int> *root = tree.getRoot();
							//std::cout << "root: " << root->value << std::endl;

							RedBlackNode <int> *node = tree.find(hundredsofthousands * 100000 + tensofthousands * 10000 + thousands * 1000 + hundreds * 100 + tens * 10 + ones);

							if (node)
							{

								RedBlackNode <int> * removed = tree.remove(node);

								if (removed) delete removed;

								if (!blackHeightTest(&tree))
								{
									std::cout << "black height error removing node " << hundredsofthousands * 100000 + tensofthousands * 10000 + thousands * 1000 + hundreds * 100 + tens * 10 + ones << std::endl;
									return 0;
								}

								RedBlackNode <int> *error;
								if (tree.getRoot())
								{
									if (tree.doubleRedError(tree.getRoot(), &error)) { std::cout << "double red error at " << error->value << " !!" << std::endl;  return 0; }
								}
							}
						}
					}
				}
			}
		}
	}

	if (!tree.getRoot()) std::cout << "EMPTY!" << std::endl;

	return 0;
	*/

	int threadCountBeforeMainLoop = 0;
	{
		std::cout << "begin" << std::endl;

		bool exitSignal = false;


		Wrapper wrapper;



		//wrapper.initialize(384, 362, 44100);
		wrapper.initialize(1920, 1080, 44100);
		//wrapper.initialize(1366, 768, 44100);



		Sprite alphabet;
		bool value = wrapper.loadSprite(&alphabet, "alphabet.bmp");

#include "basic_alphabet_table.h"
		Printer basicPrinter((int *)&basic_alphabet_table, 32, 126, &wrapper, &alphabet);

		WindowSystem windowSystem(wrapper.getXResolution(), wrapper.getYResolution(), &wrapper, &basicPrinter);





		CASLock coutLock;
		CASLock fileLock;
		lockHierarchy.addLock(&coutLock, LOCK_LEVELS::IO);
		lockHierarchy.addLock(&fileLock, LOCK_LEVELS::IO);

		CASLock IDLock;
		lockHierarchy.addLock(&IDLock, LOCK_LEVELS::IDENTITY);

		CASLock eventLock;
		lockHierarchy.addLock(&eventLock, LOCK_LEVELS::EVENTS);

		CASLock instrumentSampleLock, songModelLock;

		lockHierarchy.addLock(&instrumentSampleLock, LOCK_LEVELS::DATA_ABSTRACTIONS);
		lockHierarchy.addLock(&songModelLock, LOCK_LEVELS::DATA_ABSTRACTIONS);

		CASLock instrumentSettingsModelLock, instrumentSettingsViewLock;
		CASLock fileWindowModelLock, fileWindowViewLock;
		CASLock tracksEditModelLock, tracksEditViewLock;

		lockHierarchy.addLock(&instrumentSettingsModelLock, LOCK_LEVELS::PROGRAM_LOGIC);
		lockHierarchy.addLock(&fileWindowModelLock, LOCK_LEVELS::PROGRAM_LOGIC);
		lockHierarchy.addLock(&tracksEditModelLock, LOCK_LEVELS::PROGRAM_LOGIC);

		lockHierarchy.addLock(&instrumentSettingsViewLock, LOCK_LEVELS::GUI);
		lockHierarchy.addLock(&fileWindowViewLock, LOCK_LEVELS::GUI);
		lockHierarchy.addLock(&tracksEditViewLock, LOCK_LEVELS::GUI);


		lockHierarchy.setReady();


		//CASLock *IDLockP = &IDLock;
		//lockHierarchy.acquire(&IDLockP, 1, LOCK_LEVELS::IDENTITY);



		CASLock *coutLockP = &coutLock;
		CASLock *eventLockP = &eventLock;
		CASLock *instrumentSampleLockP = &instrumentSampleLock;

		CASLock *instrumentSettingsModelLockP = &instrumentSettingsModelLock;


		IDIssuer <SampleData> sampleDataIDIssuer(&IDLock, 5000);
		IDIssuer <Sample> sampleIDIssuer(&IDLock, 1000);

		IDIssuer <Instrument> instrumentIDIssuer(&IDLock, 200);

		IDIssuer <Track> trackIDIssuer(&IDLock, 200);

		//return 0;

		EventNotifier eventNotifier(1000000);
		CallbackConcreteRegisterer <InstrumentSampleMessage> instrumentSampleMessageRegisterer(&eventNotifier);

		InstrumentSampleModel instrumentSampleModel(&eventLock, &instrumentSampleMessageRegisterer);


		CallbackConcreteRegisterer <SongMessage> songMessageRegisterer(&eventNotifier);
		SongModel songModel(&songModelLock, &eventLock, &trackIDIssuer, &songMessageRegisterer, 6, 100000, 10000000);


		

		CallbackConcreteRegisterer <FileWindowMessage> fileWindowMessageRegisterer(&eventNotifier);

		CallbackConcreteRegisterer <FileWindowModelInternalMessage> fileWindowModelInternalRegisterer(&eventNotifier);

		FileWindowModel fileWindowModel(&fileWindowModelLock, &eventLock, &fileLock, &fileWindowMessageRegisterer, &fileWindowModelInternalRegisterer);

		

		// 6
		CallbackConcreteRegisterer <FileWindowViewInternalMessage> fileWindowViewInternalRegisterer(&eventNotifier);

		// 6
		//globalInt3 = threadCounter.getCount();
		FileWindowView fileWindowView(&fileWindowModel, &fileWindowViewInternalRegisterer, &fileWindowModelLock, &fileWindowViewLock, &eventLock, &windowSystem);

		//7 
		

		FileWindowController fileWindowController(&fileWindowModel, &fileWindowView);

		// 7

		CallbackConcreteRegisterer <InstrumentSettingsModelInternalMessage> instrumentSettingsModelInternalRegisterer(&eventNotifier);

		InstrumentSettingsModel instrumentSettingsModel(&eventLock, &coutLock, &fileLock, &instrumentSettingsModelInternalRegisterer,
			&instrumentSampleMessageRegisterer, &fileWindowMessageRegisterer, &instrumentIDIssuer, &sampleDataIDIssuer, &sampleIDIssuer, &instrumentSampleModel,
			&instrumentSettingsModelLock, &instrumentSampleLock);

		

		CallbackConcreteRegisterer <InstrumentSettingsViewInternalMessage> instrumentSettingsViewInternalRegisterer(&eventNotifier);

		InstrumentSettingsView instrumentSettingsView(&instrumentSettingsModelLock, &instrumentSettingsViewLock, &eventLock, &coutLock, &instrumentSettingsViewInternalRegisterer, &instrumentIDIssuer, &sampleIDIssuer, &instrumentSettingsModel, &windowSystem, &fileWindowView);
		InstrumentSettingsController instrumentSettingsController(&instrumentSettingsModel, &instrumentSettingsView);


		CallbackConcreteRegisterer <TracksEditModelInternalMessage> tracksEditModelInternalRegisterer(&eventNotifier);

		

		TracksEditModel tracksEditModel(&eventLock, &tracksEditModelInternalRegisterer, &songModel, &songMessageRegisterer, &tracksEditModelLock, &songModelLock);

		//TracksView(CASLock *tracksViewLock, CASLock *eventLock, CallbackConcreteRegisterer<TracksViewInternalMessage> *tracksViewInternalRegisterer, TracksModel *tracksModel, WindowSystem *windowSystem);


		CallbackConcreteRegisterer<TracksEditViewInternalMessage> tracksEditViewInternalRegisterer(&eventNotifier);

		TracksEditView tracksEditView(&tracksEditViewLock, &tracksEditModelLock, &eventLock, &tracksEditViewInternalRegisterer, &tracksEditModel, &windowSystem);

		TracksEditController tracksEditController(&tracksEditModel, &tracksEditView);

		

		MainView mainView(&instrumentSampleModel, &windowSystem, &tracksEditView);

		//MainController mainController(&sequencer, &exitSignal, &mainView, &instrumentSettingsView);
		MainController mainController(0, &exitSignal, &mainView, &instrumentSettingsView);


		mainView.openMainView();

		//threadCountBeforeMainLoop = threadCounter.getCount();
	
		WindowCanvas canvas[10];
		for (int index = 0; index < 10; index++)
		{
			WindowSprite sprite = windowSystem.getEmptyWindowSprite(1000, 300);
			canvas[index] = windowSystem.getWindowCanvas(sprite);
		}

		for (int index = 0; index < 10; index++)
		{
			windowSystem.attachWindowCanvas(canvas[index]);
		}
		
		while (!exitSignal)
		{



			CallbackInstance *instance = 0;


			{
				LockGuard eventGuard(&localLockHierarchy, &eventLockP, 1, LOCK_LEVELS::EVENTS);
				eventNotifier.newRound();
			}

			while (true)
			{



				CASLock *eventLockP = &eventLock;

				{
					LockGuard eventGuard(&localLockHierarchy, &eventLockP, 1, LOCK_LEVELS::EVENTS);
					instance = eventNotifier.getNextCallbackInstance();

				}

				if (!instance)
				{
					break;
				}


				CASLock *coutLockP = &coutLock;

				{
					LockGuard coutGuard(&localLockHierarchy, &coutLockP, 1, LOCK_LEVELS::IO);

					std::cout << "main locked coutlock at " << &coutLock << std::endl;
					std::cout << "main local lock hierarchy " << &localLockHierarchy << std::endl;
					std::cout << "call callback!" << std::endl;
				}


				bool success = instance->callCallback();



				{
					LockGuard eventGuard(&localLockHierarchy, &eventLockP, 1, LOCK_LEVELS::EVENTS);
					if (success) eventNotifier.returnCallbackInstance(instance);
					else eventNotifier.tryInstanceAgain(instance);
				}
			}

			// GUI code
			instrumentSettingsView.print();


			wrapper.clearScreen();
			wrapper.readInput();

			Input input;

			inputFunctionality(&wrapper, &input);

			if (input.KEY_PRESSED[KEY_ESCAPE]) exitSignal = true;
			/*
			if (input.KEY_PRESSED[KEY_q])
			{

				LockGuard guard(&localLockHierarchy, &instrumentSampleLockP, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

				Instrument *inst = instrumentSampleModel.getInstrument(0);

				if (inst)
				{
					Instrument instrument = *inst;
					instrument.setName("Diipadaapa");

					instrumentSampleModel.changeInstrument(instrument);
				}
			}
			*/


			/*
			if (input.KEY_PRESSED[KEY_1])
			{
				CASLock *instrumentSampleLockP = &instrumentSampleLock;
				LockGuard ISGuard(&localLockHierarchy, &instrumentSampleLockP, 1, LOCK_LEVELS::DATA_ABSTRACTIONS);

				if(instrumentSampleModel.getInstrumentAmount() > 0)
				instrumentSampleModel.deleteInstrument(0);
			}
			*/

			windowSystem.logic();
			windowSystem.draw();


			globalInt3 = threadCount.load();
			std::stringstream stream;

			stream << " ";
			stream << globalInt;
			stream << " ";
			stream << globalInt2;
			stream << " ";
			stream << globalInt3;

			std::string string = stream.str();

			basicPrinter.print((char*)string.c_str(), 10, 10, 0);
			basicPrinter.print((char*)debugString.c_str(), 10, 100, 0);

			wrapper.flip();




			/*
			for (int index = 0; index < 500; index++)
			{
				Rect dest;
				dest.x = rand() % 1900;
				dest.y = rand() % 1000;
				dest.width = rand() % 200;
				dest.height = rand() % 200;

				wrapper.drawRectangle(&dest, 0, rand() % 256, rand() % 256, rand() % 256, 255);
			}*/

			//if (exitSignal) std::cout << "threadcounter: " << threadCounter.getCount() << std::endl;
		}
	}

	//std::cout << "threadcounter: " << threadCounter.getCount() << std::endl;

	
	int oldValue = 0;
	int newValue = 0;
	do
	{
		oldValue = 0;

		do
		{
			oldValue = threadCount.load();
		} while (oldValue != 0);

		newValue = -1;

	} while (!threadCount.compare_exchange_weak(oldValue, newValue, std::memory_order_acquire));
	
	
	return 0;
}