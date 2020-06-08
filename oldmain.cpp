#include <iostream>
#include <memory>

#include "event_notifier.h"
#include "callback_registerer.h"
#include "instrument_sample_model.h"

//#include "id_issuer.h"
//#include "entry_holder.h"
#include "reference.h"
#include "selector.h"
#include "smart_ptr.h"

class dataTypeXXX
{
public:
	dataTypeXXX(std::string stringi) : stringi_(stringi)
	{
		
		std::cout << "create string type " << stringi_ << std::endl;
	}

	~dataTypeXXX()
	{
		std::cout << "delete string type " << stringi_ << std::endl;
	}

	std::string stringi_;
};

class dataTypeXXX2
{
public:
	dataTypeXXX2(int numba) : numba_(numba)
	{
		std::cout << "create number type " << numba_ << std::endl;
	}

	~dataTypeXXX2()
	{
		std::cout << "delete number type " << numba_ << std::endl;
	}

	int numba_;
};

class Juttu
{
public:
	Juttu(std::string name, CallbackConcreteRegisterer <dataTypeXXX> *registerer, CallbackConcreteRegisterer <dataTypeXXX2> *registerer2) : name_(name),
		registerer_(registerer), registerer2_(registerer2)
	{
		std::cout << "create " << name_ << " " << this << std::endl;
		registerer_->registerListener(this, Callback_);
		registerer2_->registerListener(this, Callback2_);
	}

	~Juttu() {
		registerer_->unregisterListener(this);
		registerer2_->unregisterListener(this);
		std::cout << "delete " << name_ << std::endl;
	}

	void Callback(dataTypeXXX *data)
	{
		std::cout << name_ << " " << data->stringi_ << std::endl;
	}

	static void Callback_(void *me, dataTypeXXX *data)
	{
		((Juttu*)me)->Callback(data);
	}


	void Callback2(dataTypeXXX2 *data)
	{
		std::cout << name_ << " " << data->numba_ << std::endl;
	}

	static void Callback2_(void *me, dataTypeXXX2 *data)
	{
		((Juttu*)me)->Callback2(data);
	}

private:
	std::string name_;
	CallbackConcreteRegisterer <dataTypeXXX> *registerer_;
	CallbackConcreteRegisterer <dataTypeXXX2> *registerer2_;
};

class EriJuttu
{
public:
	EriJuttu(std::string name, int kerroin, CallbackConcreteRegisterer <dataTypeXXX2> *registerer2) : name_(name), kerroin_(kerroin),
		registerer2_(registerer2)
	{
		std::cout << "create " << name_ << " " << this << std::endl;
		registerer2_->registerListener(this, Callback2_);
	}

	~EriJuttu() {
		registerer2_->unregisterListener(this);
		std::cout << "delete " << name_ << std::endl;
	}

	
	void Callback2(dataTypeXXX2 *data)
	{
		std::cout << name_ << " " << data->numba_ << " kertaa " << kerroin_ << " = " << data->numba_*kerroin_ << std::endl;
	}

	static void Callback2_(void *me, dataTypeXXX2 *data)
	{
		((EriJuttu*)me)->Callback2(data);
	}

private:
	std::string name_;
	int kerroin_;
	CallbackConcreteRegisterer <dataTypeXXX2> *registerer2_;
};

Reference <Instrument> function(Reference <Instrument> instrumentReference, CallbackConcreteRegisterer <ContainerChangeEvent> *containerChangeRegisterer, EntryHolder <Instrument> *instrumentHolder)
{
	std::cout << "function instrument reference ID: " << instrumentReference.getID() << std::endl;
	std::cout << "function instrument reference count: " << instrumentReference.getRefCount() << std::endl;

	std::cout << "new instrument in function()" << std::endl;
	Reference <Instrument> instrumentReference3(new Instrument(containerChangeRegisterer, "TestInstrument3", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), instrumentHolder);

	std::cout << "function new instrument reference ID: " << instrumentReference3.getID() << std::endl;
	std::cout << "function new instrument reference count: " << instrumentReference3.getRefCount() << std::endl;

	return instrumentReference3;
}

int main()
{
	void(*functionPointer)();

	void *voidPointer;

	functionPointer = (void(*)())EriJuttu::Callback2_;
	
	voidPointer = (void(*)())EriJuttu::Callback2_;

	std::cout << "callback2 " << &EriJuttu::Callback2 << std::endl;
	std::cout << "callback2_ " << EriJuttu::Callback2_ << std::endl;
	std::cout << "functionpointer " << functionPointer << std::endl;
	std::cout << "voidpointer " << voidPointer << std::endl;


	ClassIDIssuer issuer;
	EventNotifier notifier(100000);

	CallbackConcreteRegisterer <ContainerChangeEvent> containerChangeRegisterer(&notifier, &issuer, "ContainerChangeRegisterer");

	CallbackConcreteRegisterer <SampleEvent> sampleEventRegisterer(&notifier, &issuer, "SampleEventRegisterer");
	
	//SmartPtr <Instrument> instrument(new Instrument(&containerChangeRegisterer, "testInstrument", INSTRUMENT_TYPE::INSTRUMENT_DRUMS));
	//Selector sampleDataSelector(&containerChangeRegisterer, 0, instrument, instrument.getObject()->getSampleDataTypeName());

	//std::cout << "smart sample" << std::endl;
	SmartPtr <Sample> sample(new Sample(&sampleEventRegisterer));

	//std::cout << "create instrument" << std::endl;
	SmartPtr <Instrument> instrument(new Instrument(&containerChangeRegisterer, "testInstrument", INSTRUMENT_TYPE::INSTRUMENT_DRUMS));
	
	std::cout << "instrument refcount: " << instrument.getRefCount() << std::endl;

	//std::cout << "create container ptr" << std::endl;
	SmartPtr <Container> container = instrument;

	
	std::cout << "instrument refcount: " << instrument.getRefCount() << std::endl;
	std::cout << "container refcount: " << container.getRefCount() << std::endl;
	std::cout << "sample refcount: " << sample.getRefCount() << std::endl;


	std::cout << "get smart" << std::endl;

	SmartPtr <Sample> thisPtr = sample.getObject()->returnThis();

	std::cout << "smart refcount: " << thisPtr.getRefCount() << std::endl;
	/*
	std::shared_ptr <Sample> sharedSample(new Sample(&sampleEventRegisterer));
	std::shared_ptr <Instrument> sharedInst(new Instrument(&containerChangeRegisterer, "testInstrument", INSTRUMENT_TYPE::INSTRUMENT_DRUMS));
	std::shared_ptr <Container> sharedCont = sharedSample;
	/*
	

	std::cout << "sample refcount: " << sample.getRefCount() << std::endl;
	//std::cout << "smartfromthis refcount: " << sample.getObject()->SmartFromThis().getRefCount() << std::endl;


	//SmartPtr <SampleData> sampleData(new SampleData("sampleData", sample));


	/*
	

	SmartPtr <Instrument> smartInst(new Instrument(&containerChangeRegisterer, "testInstrument", INSTRUMENT_TYPE::INSTRUMENT_DRUMS));

	std::cout << "smartInst data " << smartInst.getObject() << std::endl;
	std::cout << "smartInst refcount " << smartInst.getRefCount() << std::endl;

	SmartPtr <Container> smartCont(smartInst);

	std::cout << "smartCont data " << smartCont.getObject() << std::endl;
	std::cout << "smartInst refcount " << smartInst.getRefCount() << std::endl;
	std::cout << "smartCont refcount " << smartCont.getRefCount() << std::endl;

	smartInst.deleteObject();

	SmartPtr <Instrument> smartInst2(new Instrument(&containerChangeRegisterer, "testInstrument2", INSTRUMENT_TYPE::INSTRUMENT_DRUMS));

	smartInst = smartInst2;

	std::cout << "smartCont data " << smartCont.getObject() << std::endl;
	std::cout << "smartInst data " << smartInst.getObject() << std::endl;
	std::cout << "smartInst2 data " << smartInst2.getObject() << std::endl;

	std::cout << "smartInst refcount " << smartInst.getRefCount() << std::endl;
	std::cout << "smartInst2 refcount " << smartInst.getRefCount() << std::endl;
	std::cout << "smartCont refcount " << smartCont.getRefCount() << std::endl;

	smartCont = smartInst2;
	
	std::cout << "smartCont data " << smartCont.getObject() << std::endl;
	std::cout << "smartInst data " << smartInst.getObject() << std::endl;
	std::cout << "smartInst2 data " << smartInst2.getObject() << std::endl;

	std::cout << "smartInst refcount " << smartInst.getRefCount() << std::endl;
	std::cout << "smartInst2 refcount " << smartInst.getRefCount() << std::endl;
	std::cout << "smartCont refcount " << smartCont.getRefCount() << std::endl;

	//smartCont = smartInst;

	*/
	

	/*
	
	IDIssuer <Instrument> instrumentIDIssuer(1000);
	EntryHolder <Instrument> instrumentHolder(&instrumentIDIssuer);

	IDIssuer <Sample> sampleIDIssuer(10000);
	EntryHolder <Sample> sampleHolder(&sampleIDIssuer);

	
	InstrumentSampleModel ISModel(&containerChangeRegisterer, &instrumentHolder, &sampleHolder);

	Selector instrumentSelector(0, &ISModel, ISModel.getInstrumentTypeName());
	Selector sampleDataSelector(&instrumentSelector, 0, ISModel.getSampleDataTypeName());
	*/



	// first tests
	/*

	std::cout << "create testRef1" << std::endl;
	Reference <Instrument> testRef1(new Instrument(&containerChangeRegisterer, "TestRefInstrument1", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), &instrumentHolder);
	std::cout << "testRef1 ID " << testRef1.getID() << std::endl;
	std::cout << "testRef1 refCount " << testRef1.getRefCount() << std::endl;
	std::cout << "testRef1 acquire " << testRef1.acquireObject() << std::endl;

	std::cout << "create testRef2" << std::endl;
	Reference <Instrument> testRef2(new Instrument(&containerChangeRegisterer, "TestRefInstrument2", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), &instrumentHolder);
	std::cout << "testRef2 ID " << testRef2.getID() << std::endl;
	std::cout << "testRef2 refCount " << testRef2.getRefCount() << std::endl;
	std::cout << "testRef2 acquire " << testRef2.acquireObject() << std::endl;

	std::cout << "assign testRef2 = testRef1" << std::endl;
	testRef2 = testRef1;
	std::cout << "testRef2 ID " << testRef2.getID() << std::endl;
	std::cout << "testRef2 refCount " << testRef2.getRefCount() << std::endl;
	std::cout << "testRef2 acquire " << testRef2.acquireObject() << std::endl;

	std::cout << "create testRef3 = testRef1" << std::endl;
	Reference <Instrument> testRef3 = testRef1;
	std::cout << "testRef3 ID " << testRef3.getID() << std::endl;
	std::cout << "testRef3 refCount " << testRef3.getRefCount() << std::endl;
	std::cout << "testRef3 acquire " << testRef3.acquireObject() << std::endl;

	std::cout << "create testRef4 = testRef1" << std::endl;
	Reference <Instrument> testRef4 = testRef1;

	std::cout << "assign testRef4 = testRef1" << std::endl;
	testRef4 = testRef1;
	std::cout << "testRef4 ID " << testRef4.getID() << std::endl;
	std::cout << "testRef4 refCount " << testRef4.getRefCount() << std::endl;
	std::cout << "testRef4 acquire " << testRef4.acquireObject() << std::endl;

	std::cout << "testRef1 delete object" << std::endl;
	testRef1.deleteObject();
	std::cout << "testRef4 ID " << testRef4.getID() << std::endl;
	std::cout << "testRef4 refCount " << testRef4.getRefCount() << std::endl;
	std::cout << "testRef4 acquire " << testRef4.acquireObject() << std::endl;
	*/

	/*
	Reference <Instrument> instrumentReference(new Instrument(&containerChangeRegisterer, "TestInstrument", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), &instrumentHolder);
	Reference <Instrument> instrumentReference2(new Instrument(&containerChangeRegisterer, "TestInstrument2", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), &instrumentHolder);

	std::cout << "instrument ref 1 ID: " << instrumentReference.getID() << std::endl;
	std::cout << "instrument ref 2 ID: " << instrumentReference2.getID() << std::endl;

	std::cout << "instrument ref 1 refcount: " << instrumentReference.getRefCount() << std::endl;
	std::cout << "instrument ref 2 refcount: " << instrumentReference2.getRefCount() << std::endl;
	Reference <Instrument> instrumentReference3 = instrumentReference2;
	std::cout << "ref 3 = ref 2" << std::endl;

	std::cout << "instrument ref 3 ID: " << instrumentReference3.getID() << std::endl;


	std::cout << "ref 1 count: " << instrumentReference.getRefCount() << std::endl;
	std::cout << "ref 2 count: " << instrumentReference2.getRefCount() << std::endl;
	std::cout << "ref 3 count: " << instrumentReference3.getRefCount() << std::endl;

	std::cout << "before function()" << std::endl;
	Reference <Instrument> returnedReference = function(instrumentReference3, &containerChangeRegisterer, &instrumentHolder);

	std::cout << "after function()" << std::endl;

	std::cout << "returned ref ID: " << returnedReference.getID() << std::endl;
	std::cout << "returned ref count: " << returnedReference.getRefCount() << std::endl;

	std::cout << "ref 1 count: " << instrumentReference.getRefCount() << std::endl;
	std::cout << "ref 2 count: " << instrumentReference2.getRefCount() << std::endl;
	std::cout << "ref 3 count: " << instrumentReference3.getRefCount() << std::endl;

	Reference <Instrument> instrumentReference4(new Instrument(&containerChangeRegisterer, "TestInstrument4", INSTRUMENT_TYPE::INSTRUMENT_DRUMS), &instrumentHolder);

	std::cout << "instrument ref 4 ID: " << instrumentReference4.getID() << std::endl;
	std::cout << "ref 4 count: " << instrumentReference4.getRefCount() << std::endl;


	std::cout << "testinstrument 2 address through ref 2: " << instrumentReference2.acquireObject() << std::endl;
	std::cout << "testinstrument 2 address through ref 3: " << instrumentReference3.acquireObject() << std::endl;
	std::cout << "manually delete testinstrument 2" << std::endl;
	instrumentReference2.deleteObject();
	std::cout << "testinstrument 2 address: " << instrumentReference2.acquireObject() << std::endl;
	std::cout << "testinstrument 2 address through ref 2: " << instrumentReference2.acquireObject() << std::endl;
	std::cout << "testinstrument 2 address through ref 3: " << instrumentReference3.acquireObject() << std::endl;
	std::cout << "ref 2 count: " << instrumentReference2.getRefCount() << std::endl;
	std::cout << "ref 3 count: " << instrumentReference3.getRefCount() << std::endl;


	std::cout << "returned ref = ref 4:" << std::endl;

	returnedReference = instrumentReference4;

	std::cout << "XXX" << std::endl;

	//Instrument drums(&containerChangeRegisterer, "Drums", INSTRUMENT_TYPE::INSTRUMENT_DRUMS);
	//InstrumentSampleModel instrumentSampleModel(&containerChangeRegisterer);
	*/


	
	/*
	CallbackConcreteRegisterer <dataTypeXXX> XXXregisterer(&notifier, &issuer, "XXXRegisterer");

	CallbackConcreteRegisterer <dataTypeXXX2> XXX2registerer(&notifier, &issuer, "XXX2Registerer");
	
	
	Juttu juttu("eka", &XXXregisterer, &XXX2registerer);
	Juttu juttu2("toka", &XXXregisterer, &XXX2registerer);

	EriJuttu kertoja("Kertoja", 3, &XXX2registerer);

	XXXregisterer.notify(new dataTypeXXX("data 1"));
	XXX2registerer.notify(new dataTypeXXX2(56));

	XXXregisterer.notify(new dataTypeXXX("data 2"));
	XXXregisterer.notify(new dataTypeXXX("data 3"));
	XXXregisterer.notify(new dataTypeXXX("data 4"));

	XXXregisterer.notify(new dataTypeXXX("data 5"));
	XXXregisterer.notify(new dataTypeXXX("data 6"));
	XXXregisterer.notify(new dataTypeXXX("data 7"));
	*/
	

	CallbackInstance *instance = 0;

	while (instance = notifier.getNextCallbackInstance())
	{
		instance->callCallback();
		notifier.returnCallbackInstance(instance);
	}

	


	/*
	
	CallbackInstance *instance = 0;

	
	
	while (instance = notifier.getNextCallbackInstance())
	{
		instance->callCallback();
		notifier.returnCallbackInstance(instance);
	}
	*/



	std::cout << "END" << std::endl;



	//notifier.callCallbacks();

	return 0;
}