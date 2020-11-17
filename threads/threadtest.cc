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
#include "synch.h"
 
// testnum is set in main.cc
int testnum = 1 ;


//int empty = 10;
int itemnum = 0;

Semaphore *mutex = new Semaphore("Prod-Cons-mutex",1);
Semaphore *empty = new Semaphore("Prod-Cons-syn-empty",10);
Semaphore *full = new Semaphore("Prod-Cons-syn-full",0);

Lock *syntestlock = new Lock("Prod-Cons");

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
    for (num = 0; num < 5; num++) 
    {
            //printf("*** thread %d looped %d times\tCUR_Thread:%s\t Thread_used_time_slice:%d\tCURSystem:%d\t \n", which, num,currentThread->getName(),currentThread->used_time_slice,stats->systemTicks);
            //currentThread->setPriority(currentThread->getPriority()-1);
            printf("Thread t%d\t syntestnum1:%d\t syntestnum2:%d\n",which,syntestnum1,syntestnum2);
            currentThread->Yield();  
    }
    //globalThreadManager->ShowListInfo();
}

//---------------------------------------------------------------------
//生产者消费者
//---------------------------------------------------------------------
void Producer(int which)
{
    int loop = 1;
    while(loop <=5)
    {
    //printf("*** thread %d looped %d times\tCUR_Thread:%s\t Thread_used_time_slice:%d\tCURSystem:%d\t \n", which, loop,currentThread->getName(),currentThread->used_time_slice,stats->systemTicks);
    empty->P();
    currentThread->Yield();
    mutex->P();
    currentThread->Yield();
    printf("Producer%d produece an item\t itemnum:%d empty:%d full:%d\n",which,itemnum,empty->getValue(),full->getValue());
    itemnum++;
    currentThread->Yield();
    mutex->V();
    currentThread->Yield();
    full->V();
    loop++;
    printf("loops:%d\n", loop);
    }
}
void Consumer(int which)
{
    int loop = 1;
    while(loop<=10){
    full->P();
    currentThread->Yield();
    mutex->P();
    currentThread->Yield();
    itemnum--;
    printf("Consume%d consume an item\t itemnum:%d empty:%d full:%d\n",which,itemnum,empty->getValue(),full->getValue());
    mutex->V();
    currentThread->Yield();
    empty->V();
    loop++;
    }
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
    Thread *t1 = new Thread("Producer1");
    //t1->setPriority(9);
    Thread *t2 = new Thread("Consumer1");
    //t2->setPriority(8);
    Thread *t3 = new Thread("Consumer2");
    //t3->setPriority(4);
    Thread *t4 = new Thread("Producer2");

    t1->Fork(Producer, 1);
    t2->Fork(Consumer, 1);
    t3->Fork(Consumer, 2);
    t4->Fork(Producer, 2);

    globalThreadManager->ShowListInfo();
    
    //SimpleThread(0);
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

