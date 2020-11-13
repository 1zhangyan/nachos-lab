// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

// testnum is set in main.cc
int testnum = 1 ;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    printf("Come into Thread t%d\n",which);
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\tCUR_Thread:%s\t Thread_used_time_slice:%d\tCURSystem:%d\t \n", which, num,currentThread->getName(),currentThread->used_time_slice,stats->systemTicks);
        //currentThread->setPriority(currentThread->getPriority()-1);

        currentThread->Yield();
        
    }
    //globalThreadManager->ShowListInfo();
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");



    Thread *t1 = new Thread("t1");
    //t1->setPriority(9);
    Thread *t2 = new Thread("t2");
    //t2->setPriority(8);
    Thread *t3 = new Thread("t3");
    //t3->setPriority(4);


    t1->Fork(SimpleThread, 1);
    t2->Fork(SimpleThread, 2);
    t3->Fork(SimpleThread, 3);

    globalThreadManager->ShowListInfo();
    
    SimpleThread(0);
    //printf("CUR NAME: %s\n ",currentThread->getName());

    
}
//--------------------------------------
//ThreadTest2
//
//--------------------------------------
void ThreadTest2()
{
   // printf("RRRRRRRR");
    for(int i = 0 ; i <130; i++)
    {
        Thread *t;
        t = new Thread("test2");
    }
    globalThreadManager->ShowListInfo();
}



//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------


void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    case 2:
    ThreadTest2();
    default:
	printf("No test specified.\n");
	break;
    }
}

