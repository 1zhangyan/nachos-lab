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


//公共锁和信号量
Semaphore *mutex = new Semaphore("mutex",1);
Lock *syntestlock = new Lock("Lock");

//生产者消费者 变量 锁  和信号量
int itemnum = 0;
Semaphore *empty = new Semaphore("Prod-Cons-syn-empty",10);
Semaphore *full = new Semaphore("Prod-Cons-syn-full",0);
Lock *pro_con_lock = new Lock("Prod-Cons-syn-lock");
Condition *con = new Condition("Prod-Cons-cond");
 

//哲学家就餐问题锁和信号量
Semaphore chopsticks[4] = {Semaphore("chop0",1),Semaphore("chop1",1),Semaphore("chop2",1),Semaphore("chop3",1) };

//屏障机制 条件变量
Lock *barrier_lock = new Lock("barrier_lock");
Condition *barrier_cond = new Condition("barrier_cond");
int countnum = 0;
int threadnum = 0;
int barrier_flag[4];

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
            printf("Thread t%d\t loop num %d\t \n",which,num);
            currentThread->Yield();  
    }
    //globalThreadManager->ShowListInfo();
}

//---------------------------------------------------------------------
//---------------------------
//生产者消费者   信号量版
//---------------------------
//---------------------------------------------------------------------
void Producer_semaphore(int which)
{
    int loop = 1;
    while(loop <=5)
    {
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
void Consumer_semaphore(int which)
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
//-------------------------------------------
//----------------生产者消费者 条件变量版------
//-------------------------------------------
void Producer_cond(int which)
{
    int loop = 1;
    while(loop <=5)
    {
        pro_con_lock->Acquire();
        currentThread->Yield();
        itemnum++;
        printf("Producer%d produce an item. Itemnum:%d \n" ,which, itemnum);
        currentThread->Yield();
        pro_con_lock->Release();
        currentThread->Yield();
        con->Broadcast(pro_con_lock);
        loop++;
    }
}
void Consumer_cond(int which)
{
    int loop = 1;
    while(loop <=5)
    {
        pro_con_lock->Acquire();
        currentThread->Yield();
        if (itemnum<=0)
        {
            con->Wait(pro_con_lock);
        }
        currentThread->Yield();
        itemnum--;
        printf("Consumer%d consume an item. Itemnum:%d \n" , which , itemnum);
        currentThread->Yield();
        pro_con_lock->Release();
        currentThread->Yield();
        
        loop++;
    }
}

void
ThreadTest3()
{
    DEBUG('t', "Entering ThreadTest1");
    Thread *t1 = new Thread("Producer1");
    Thread *t2 = new Thread("Consumer1");
    Thread *t3 = new Thread("Consumer2");
    Thread *t4 = new Thread("Producer2");

    t2->Fork(Consumer_cond, 1);
    t1->Fork(Producer_cond, 1);
    t4->Fork(Producer_cond, 2);
    t3->Fork(Consumer_cond, 2);
    
    for(int  i = 0 ; i< 10000 ; i++)
    {
        currentThread->Yield();
    }
    globalThreadManager->ShowListInfo();
}

//-------------------------------------------
//-------------------哲学家就餐---------------
//-------------------------------------------
void Philosopher_deadlock(int which)
{
    int i = 0;
    while(i<20)
    {
        printf("Philosopher%d Want to Get Chopsticks%d\n" , which ,(which-1+4)%4 );
        chopsticks[(which-1+4)%4].P();
        printf("Philosopher%d Get Chopsticks%d\n" , which ,(which-1+4)%4 );
        currentThread->Yield();

        printf("Philosopher%d Want to Get Chopsticks%d\n" , which ,which );
        chopsticks[which].P();
        printf("Philosopher%d Get Chopsticks%d\n" , which ,which );
        currentThread->Yield();

        printf("Philosopher%d Having A Meal\n" , which);

        currentThread->Yield();
        chopsticks[(which-1+4)%4].V();
        printf("Philosopher%d Return Chopsticks%d\n" , which ,(which-1+4)%4 );
        currentThread->Yield();


        chopsticks[which].V();
        printf("Philosopher%d Return Chopsticks%d\n" , which ,which);
        i++;
        currentThread->Yield();
    }
}

void server_get(int which)
{
    printf("Philosopher%d Want to Get Chopsticks%d\n" , which ,(which-1+4)%4 );
    chopsticks[(which-1+4)%4].P();
    printf("Philosopher%d Get Chopsticks%d\n" , which ,(which-1+4)%4 );
    currentThread->Yield();
    printf("Philosopher%d Want to Get Chopsticks%d\n" , which ,which );
    chopsticks[which].P();
    printf("Philosopher%d Get Chopsticks%d\n" , which ,which );
    //currentThread->Yield();

}

void server_ret(int which)
{
    //currentThread->Yield();
    chopsticks[(which-1+4)%4].V();
    printf("Philosopher%d Return Chopsticks%d\n" , which ,(which-1+4)%4 );
    currentThread->Yield();
    chopsticks[which].V();
    printf("Philosopher%d Return Chopsticks%d\n" , which ,which);
   // currentThread->Yield();
}

void Philosopher(int which)
{
    int i = 0;
    while(i<10)
    {
        mutex->P();
        server_get(which);
        mutex->V();
        printf("Philosopher%d Having A Meal\n" , which);
        mutex->P();
        server_ret(which);
        mutex->V();
        i++;   
    }
}

void ThreadTest2()
{

    DEBUG('t', "Entering ThreadTest2");
    Thread *t1 = new Thread("Philosopher0");
    Thread *t2 = new Thread("Philosopher1");
    Thread *t3 = new Thread("Philosopher2");
    Thread *t4 = new Thread("Philosopher3");

    t1->Fork(Philosopher, 0);
    t2->Fork(Philosopher, 1);
    t3->Fork(Philosopher, 2);
    t4->Fork(Philosopher, 3);

    for(int  i = 0 ; i< 10000 ; i++)
    {
        currentThread->Yield();
    }
}
//------------------------------
//Barrier屏障机制
//Threadtest3
//int countnum = 0;
//int threadnum = 0;
//int barrier_flag[4];
//--------------------------------

void init_flag()
{
    for(int i = 0 ; i < 4 ; i ++)
    barrier_flag[i] = 0;
}

bool all_thread_ready()
{
    for (int i = 0 ; i < 4 ; i++)
    {
        if (barrier_flag[i]!=1)
        {
            return false;
        }
    }
    return true;
}

void barrier_function(int which)//Realize a barrier demo
{
    int thiscountnum = which;
    int i = 0 ; 
    while(i < 2)
    { 
        while(thiscountnum < 5)
        {
            currentThread->Yield();
            thiscountnum++;
        }
        i++;
        barrier_lock->Acquire();
        barrier_flag[which] = 1;
        //printf(" currentthread %s flag %d\n",currentThread->getName() , all_thread_ready());
        if (!all_thread_ready())
        {
            barrier_cond->Wait(barrier_lock);
        }
        countnum += thiscountnum;
        printf("Add barrier function%d countnum = %d \n" ,which, countnum);
        barrier_flag[which] = 0;
        currentThread->Yield();
        barrier_lock->Release();
        currentThread->Yield();
        barrier_cond->Broadcast(barrier_lock);
        thiscountnum = which;
    }
}

void ThreadTest4()
{
    
    init_flag();
    DEBUG('t', "Entering ThreadTest3");
    Thread *t1 = new Thread("barrier_function0");
    Thread *t2 = new Thread("barrier_function1");
    Thread *t3 = new Thread("barrier_function2");
    Thread *t4 = new Thread("barrier_function3");

    t1->Fork(barrier_function, 0);
    t2->Fork(barrier_function, 1);
    t3->Fork(barrier_function, 2);
    t4->Fork(barrier_function, 3);

    for(int  i = 0 ; i< 10000 ; i++)
    {
        currentThread->Yield();
    }
    globalThreadManager->ShowListInfo();
}
//-------------------------------------------------
//KFIFO 1
//-------------------------------------------------


void ThreadTest5()
{
    DEBUG('t', "Entering ThreadTest1");
    unsigned int a = 9;
    a= a&(8-1);
    printf("%d\n",a);
}




//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------
void
ThreadTest1()
{
    /*
    DEBUG('t', "Entering ThreadTest1");
    Thread *t1 = new Thread("SimpleThread");
    //t1->setPriority(9);
    Thread *t2 = new Thread("SimpleThread");
    //t2->setPriority(8);
    Thread *t3 = new Thread("SimpleThread");
    //t3->setPriority(4);
    Thread *t4 = new Thread("SimpleThread");

    t2->Fork(SimpleThread, 2);
    t1->Fork(SimpleThread, 1);
    t4->Fork(SimpleThread, 4);
    t3->Fork(SimpleThread, 3);
    globalThreadManager->ShowListInfo();
    SimpleThread(0);*/
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
    break;
    case 3:
    ThreadTest3();
    break;
    case 4:
    ThreadTest4();
    break;
    case 5:
    ThreadTest5();
    break;
    default:
	printf("No test specified.\n");
	break;
    }
}