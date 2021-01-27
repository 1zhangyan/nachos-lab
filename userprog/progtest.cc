// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

//==========================Fork函数执行的函数体==================
void simple(int which)
{
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();	
    printf("SecondThread Userpro Start!\n");
    machine->Run();
}


//=========================================



void
StartProcess(char *filename)
{
    printf("Main Thread strat and First User thread Start!\n");
    currentThread->filename = filename;
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }


    space = new AddrSpace(executable);    
    currentThread->space = space;

    delete executable;			// close file

/*
//===================== Add second thread code===============
OpenFile *executable2 = fileSystem->Open(filename);
AddrSpace *space2;
if (executable2 == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }
    space2 = new AddrSpace(executable2);    
    Thread * SecondThread = new Thread("SecondThread");
    SecondThread->space = space2;
    delete executable2;
    SecondThread->Fork(simple,1);		


//===========================================================
*/


    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(FALSE);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static SynchConsole *synchConsole;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    //code pre 
    char ch;
    /*
    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }*/

  //Exercise Add Code 
  synchConsole = new SynchConsole(in ,out);
  for(;;)
  {
      ch = synchConsole->GetChar();
      synchConsole->PutChar(ch);
      if (ch == 'q') return ;
  }
}
