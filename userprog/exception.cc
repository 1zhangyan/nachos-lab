// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.


#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "filesys.h"


#define SWAPALG 2




//#define TLBSize 4
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void syscallFork(int spacePointer)
{
    printf("Come into syscallFork function  current thread: %s\n", currentThread->getName());
    currentThread->space = (AddrSpace*)spacePointer;
    machine-> WriteRegister(PCReg,currentThread->space->funcAddr);
    machine-> WriteRegister(NextPCReg,currentThread->space->funcAddr+sizeof(int));
    machine->Run();
}


void StartUerPro(int nameAddr)
{
    printf("This is func startuserpro\n");
    char filename[20];
    int offset = 0;
    int data;    
    while(true)
    {
        machine->ReadMem(nameAddr + offset , 1 , &data);
        if (data == 0)
        {
            filename[offset] = '\0';
            break;
        }
        filename[offset] = char(data);
        offset+=1;
    }
    printf("Exec filename from the given Addr : ");
    for(int i = 0 ; i < 20 ; i++)
        printf("%c",filename[i]);
    printf("\n");


    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;
    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }
    space = new AddrSpace(executable);    
    currentThread->space = space;
    delete executable;			
    space->InitRegisters();		
    space->RestoreState();		
    machine->Run();			
    ASSERT(FALSE);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	   
        printf("this is halt syscall  current thread is : %s \n" , currentThread->getName());
        DEBUG('a', "Shutdown, initiated by user program.\n");
   	    interrupt->Halt();
    } 
    else if((which == SyscallException) && (type == SC_Exit))
    {
        printf("this is Exit syscall\n");
        int status = machine->ReadRegister(4);
        printf("Thread %s Exit with status : %d \n",currentThread->getName() , status);
        //machine->clear();
        machine->PCOneTick();
        currentThread->Finish();
    }
    else if((which == SyscallException) && (type == SC_Exec))
    {
        printf("this is Exec syscall \n");
        int nameAddr = machine->ReadRegister(4);
        printf("Read filename Addr from Register4 : NameAddr = %d\n",nameAddr);

        Thread *userThread = new Thread("StartUerPro");
        userThread->Fork(StartUerPro , nameAddr);
        printf("StartUserPro Pid %d\n", userThread->getPid());
        machine->WriteRegister(2 , userThread->getPid());
        machine->PCOneTick();   
    }
    else if((which == SyscallException) && (type == SC_Join))
    {
        printf("This is Join syscall\n");
        int threadid = machine->ReadRegister(4);
        printf("Get thread %d\n",threadid);
        while(globalThreadManager->GlobalThreadList[threadid]!=NULL)
        {
            currentThread->Yield();
        }
        printf("Thread %d has exited\n",threadid);
        //globalThreadManager->ShowListInfo();
        machine->PCOneTick();
    }


    else if((which == SyscallException) && (type == SC_Fork))
    {
             printf("this is Fork syscall \n");
            Thread *syscallForkThread = new Thread("syscallForkThread");
            int funcAddr = machine->ReadRegister(4);
            currentThread->space->funcAddr = funcAddr;
            syscallForkThread->Fork(syscallFork , int(currentThread->space));
            machine->PCOneTick();
    }



    else if((which == SyscallException) && (type == SC_Yield))
    {
        printf("This is Yield Syscall \n");
        machine->PCOneTick();
        currentThread->Yield();
    }

    else if((which == SyscallException) && (type == SC_Create))
    {
        printf("This is Create Syscall \n");
        int nameAddr = machine->ReadRegister(4);
        printf("Read filename Addr from Register4 : NameAddr = %d\n",nameAddr);
        char name[20];
        int offset = 0;
        int data;
        
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        printf("Read filename from the given Addr : ");
        for(int i = 0 ; i < 10 ; i++)
        printf("%c", name[i]);
        printf("\nThen Create the file\n");
        fileSystem->Create(name , 128);
        machine->PCOneTick();
    }   
    else if((which == SyscallException) && (type == SC_Open))
    {
            printf("This is Open Syscall \n");
            int nameAddr = machine->ReadRegister(4);
            printf("Read filename Addr from Register4 : NameAddr = %d\n",nameAddr);
            char name[10];
            int offset = 0;
            int data;
            while(true)
            {
                machine->ReadMem(nameAddr + offset , 1 , &data);
                if (data == 0)
                {
                    name[offset] = '\0';
                    break;
                }
                name[offset] = char(data);
                offset+=1;
            }
            printf("Read filename from the given Addr : ");
            for(int i = 0 ; i < 10 ; i++)
            printf("%c", name[i]);
            printf("\nThen Open the file\n");
            OpenFile* openfile = fileSystem->Open(name);
            printf("Now Get the FileID(Handler) is : %d\n" , int(openfile));
            printf("Write this value into rg2 as the retrurn value\n" , openfile);
            machine->WriteRegister(2 , int(openfile));
            machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Read))
    {
        //int Read(char *buffer, int size, OpenFileId id);
        //printf("This is Read Syscall \n");

        int bufferAddr = machine->ReadRegister(4);
        int size = machine->ReadRegister(5);
        int fid = machine->ReadRegister(6);
        //printf("Read filename Addr from Register4 : bufferAddr = %d Read size from Register5 : size = %d Read fileId from Register6 : id = %d  \n",bufferAddr , size , fid);
        char content[size];

        if (fid == ConsoleInput)
        {
            for (int i = 0 ; i < size; i++)
            {
                content[i] = getchar();
                machine->WriteMem(bufferAddr+i , 1 , int(content[i]));
            }
               
        }
        else
        {
            OpenFile *openfile = (OpenFile*) fid;
            int readNum = openfile->Read(content , size);
            printf("Read content into from file:");
            for(int i = 0 ; i < size ; i ++)
            {
                machine->WriteMem(bufferAddr+i , 1 , int(content[i]));
                printf("%c",content[i]);
            }
            printf("\nNow write the retNum = %d into Rg2\n" , readNum);
            machine->WriteRegister(2,readNum);
        }
        machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Write))
    {
        //printf("This is Write Syscall \n");
        int bufferAddr = machine->ReadRegister(4);
        int size = machine->ReadRegister(5);
        int fid = machine->ReadRegister(6);
        //printf("Read filename Addr from Register4 : bufferAddr = %d Read size from Register5 : size = %d Read fileId from Register6 : id = %d  \n",bufferAddr , size , fid);
        char content[size];
        int data;
        if(fid == ConsoleOutput)
        {
            for (int i = 0 ; i < size; i++)
            {
                machine->ReadMem(bufferAddr + i , 1 , &data);
                putchar(data);
            }
        }
        
        else{
            for(int i = 0 ; i < size; i++)
            {

                machine->ReadMem(bufferAddr + i , 1 , &data);
                content[i] = char(data);
                printf("data = %c\n", data);
            }
            printf("Now write the content into the file\n");
            OpenFile *openfile = (OpenFile*) fid;
            openfile->Write(content , size);
        }
        machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Close))
    {

        printf("This is Close Syscall \n");
        int fid = machine->ReadRegister(4);
        printf("Read fid from Register4 fid = %d\n",fid);
        OpenFile *openfile = (OpenFile*)fid;
        printf("Now Delete the openfile obj to close the file\n");
        delete openfile;
        machine->PCOneTick();
    }

    else if((which == SyscallException) && (type == SC_Ls))
    {
        //printf("SC_LS\n");
        system("ls");
        machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Pwd))
    {
        //printf("SC_Pwd\n");
        system("pwd");
        machine->PCOneTick();

    }
    else if((which == SyscallException) && (type == SC_Cd))
    {
        int nameAddr = machine->ReadRegister(4);
        char name[20];
        int offset = 0;
        int data;
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        chdir(name);

        machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Rmf))
    {
        int nameAddr = machine->ReadRegister(4);
        char name[20];
        int offset = 0;
        int data;
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        remove(name);
        machine->PCOneTick();
    }
    
    else if((which == SyscallException) && (type == SC_Rmd))
    {
        int nameAddr = machine->ReadRegister(4);
        char name[20];
        int offset = 0;
        int data;
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        remove(name);
        machine->PCOneTick();
    }


    else if((which == SyscallException) && (type == SC_Mkdir))
    {
        int nameAddr = machine->ReadRegister(4);
        char name[20];
        int offset = 0;
        int data;
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        mkdir(name,0777);
        machine->PCOneTick();
    }
    else if((which == SyscallException) && (type == SC_Touch))
    {
        int nameAddr = machine->ReadRegister(4);
        char name[20];
        int offset = 0;
        int data;
        while(true)
        {
            machine->ReadMem(nameAddr + offset , 1 , &data);
            if (data == 0)
            {
                name[offset] = '\0';
                break;
            }
            name[offset] = char(data);
            offset+=1;
        }
        fileSystem->Create(name , 128);
        machine->PCOneTick();
    }
    
    else 
    {
       // printf("Unexpected user mode exception %d %d\n", which, type);
       // ASSERT(FALSE);
       interrupt->Halt();
    }
}
