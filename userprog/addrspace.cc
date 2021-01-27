// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable )
{
    
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    bool success_create_vm = fileSystem->Create("VirtualMemory", size);
    OpenFile *vm = fileSystem->Open("VirtualMemory");
    
    char *virtualMemory_temp;
    virtualMemory_temp = new char[size];
    for (i = 0; i < size; i++)
        virtualMemory_temp[i] = 0;
    if (noffH.code.size > 0) {
        executable->ReadAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                           noffH.code.size, noffH.code.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                    noffH.code.size, noffH.code.virtualAddr*PageSize);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "\tCopying data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                    noffH.initData.size, noffH.initData.virtualAddr*PageSize);
    }
   DEBUG('a', "Copy all things to VM vm start at : %d\n",noffH.code.virtualAddr*PageSize);
/*
pageTable = new TranslationEntry[numPages];
for (i = 0; i < numPages; i++) {
	machine->pageTable[i].virtualPage = i;
    machine->pageTable[i].physicalPage = i;
	machine->pageTable[i].valid = TRUE;
	machine->pageTable[i].use = FALSE;
	machine->pageTable[i].dirty = FALSE;
	machine->pageTable[i].readOnly = FALSE;  			
    }
/*


//···············分配页表·································
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	
    //==============找到bitmap中的物理页框号来分配==============
    int NewPhysicPage = machine->MemoryMap->Find();
    if(NewPhysicPage == -1){
        printf("NO PhysicPage can be allocatr\n");
        ASSERT(FALSE);}
    pageTable[i].physicalPage = NewPhysicPage;
    //=============              ==============================
    
    //pageTable[i].physicalPage = i;
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    }
    printf("After Allocating Addr Space:\n");
    machine->MemoryMap->Print();//分配完毕时候查看系统内存
//····················分配页表END·························

*/

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    

/*
//----------------------------------------------一次性装入所有内容进主存Start-----------------------

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        //printf("noffH.code.size > 0\n");
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);

//====================装填用户程序空间的代码段================
int code_pos = noffH.code.inFileAddr;
for(int j = 0 ; j < noffH.code.size ; j++)
{
    int tmp_vpn = (noffH.code.virtualAddr + j )/PageSize;
    int tmp_offset = (noffH.code.virtualAddr + j )%PageSize;
    int paddr = pageTable[tmp_vpn].physicalPage*PageSize + tmp_offset;
    executable->ReadAt(&(machine->mainMemory[paddr]),1, code_pos++);
}
//==========================================================

        //executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        //printf("noffH.code.size < 0\n");
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);


//========================装填用户程序空间的初始化数据段=========
int data_pos = noffH.initData.virtualAddr;
for(int j = 0 ; j < noffH.code.size ; j++){
    int tmp_vpn = (noffH.code.virtualAddr + j )/PageSize;
    int tmp_offset = (noffH.code.virtualAddr + j )%PageSize;
    int paddr = pageTable[tmp_vpn].physicalPage*PageSize + tmp_offset;
    executable->ReadAt(&(machine->mainMemory[paddr]),1, data_pos++);
}
//============================================================
        //executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),noffH.initData.size, noffH.initData.inFileAddr);
    }
//----------------------一次性装入所有内容进主存END------------------------------------------------------------

*/

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    //======================bitMap已分配内存回收=======================
    /*for (int i = 0; i < numPages; i++) {
    machine->MemoryMap->Clear(pageTable[i].physicalPage);
    }
    printf("After Recollect Thread %s the Allocated Addr Space:\n",threadToBeDestroyed->getName());
    machine->MemoryMap->Print();*/
    //=========================================================
   //delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    #ifdef USE_TLB
    //====================TLB失效=====================
    for (int i = 0; i < TLBSize; i++)
	machine->tlb[i].valid = FALSE;
    //===============================================
    #endif
   /* for(int i = 0 ; i < NumPhysPages ; i++)
    {
        machine->pageTable[i].physicalPage =i;
        machine->pageTable[i].virtualPage =-1;
        machine->pageTable[i].use = FALSE;
        machine->pageTable[i].readOnly = FALSE;
        machine->pageTable[i].dirty = FALSE;
        machine->pageTable[i].valid = FALSE;
    }*/
}
