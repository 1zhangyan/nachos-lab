// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
    printf("%s Go to sleep\n" , currentThread->getName());
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}


//=======================================LOCK===================================
// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) 
{
    name  = debugName;
    lock = new Semaphore(debugName , 1);
    heldthread = NULL;
}
Lock::~Lock() 
{
    delete lock;
}

//------------------------------------------------------
void Lock:: Acquire() 
{   
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    lock->P();
    heldthread = currentThread;
    //(void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------
void Lock::Release() 
{
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    if(isHeldByCurrentThread()==true)
    {
        heldthread = NULL;
        lock->V();
    }  
    //(void) interrupt->SetLevel(oldLevel);
}

bool Lock::isHeldByCurrentThread()
{
    return (currentThread==heldthread);
}




//========================================Condition Variable==========================
Condition::Condition(char* debugName) 
{
    waitinglist = new List();
}
Condition::~Condition() 
{ 
    delete waitinglist;
}
//--------------------------------------
void Condition::Wait(Lock* conditionLock) 
{
    //ASSERT(FALSE);
    
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    conditionLock->Release();
    waitinglist->Append(currentThread);
    currentThread->Sleep();
    conditionLock->Acquire();
    (void) interrupt->SetLevel(oldLevel);
    
}

//--------------------------------------
void Condition::Signal(Lock* conditionLock)
{ 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    if(!waitinglist->IsEmpty())
    {
        Thread *next = (Thread*)waitinglist->Remove();
        scheduler->ReadyToRun(next);
    }
    (void) interrupt->SetLevel(oldLevel);
    
}

//--------------------------------------
void Condition::Broadcast(Lock* conditionLock)
{
    
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    while(!waitinglist->IsEmpty())
    {
        Signal(conditionLock);
    }
    (void) interrupt->SetLevel(oldLevel);
    
}


//----------------------------------------------------------
//------- Class Kfifo Realization Code ----------------------
/*     Definintion of the Kfifo ::
class Kfifo{
public:
    unsigned int in; // queue head pointer
    unsigned int out; // queue tail pointer
    Kfifo(int size); // constructor to initialize the var
    ~Kfifo(); // destructor
    int get_all_size(); // return the size of the queue buffer (generally should be power(2))
    int get_remine_size(); // return the size can be used in queue buffer
    bool put_into_queue(char* item , int size); // put an item into the queue buffer
    bool get_from_queue(char* item , int size); // get out of an item from queue buffer
private:
    int size;
    char *buffer;
};
*/ 
//-----------------------------------------------------------
/*
Kfifo::Kfifo(int size)
{
    in = 0;
    out = 0;
    size = size;
    char *buffer = new char[size];
}

Kfifo::~Kfifo()
{
    delete buffer;
}

int Kfifo::get_all_size()
{
    return size;
}
int Kfifo::get_remine_size()
{
    
}

bool Kfifo::get_from_queue(char* item , int size)
{
    size = size>get_remine_size()?get_remine_size():size;
    for(int i = 0 ; i < size ; i++)
    {
        item[i]  = buffer[out++];     //out should be dealt with outofmem
    }
    return true;
}

bool Kfifo::put_into_queue(char* item , int size)
{

}
*/