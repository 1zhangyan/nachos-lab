// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include <time.h>
#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "bitmap.h"

#define NumDirect 	((SectorSize - 6 * sizeof(int)) / sizeof(int)) // 一个文件26个扇区
#define MaxFileSize 	(NumDirect * SectorSize)//最大文件长度


// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:
    bool Allocate(BitMap *bitMap, int fileSize);// Initialize a file header, 
						//  including allocating space 
						//  on disk for the file data
    void Deallocate(BitMap *bitMap);  		// De-allocate this file's 
						//  data blocks

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk

    int ByteToSector(int offset);	// Convert a byte offset into the file
					// to the disk sector containing
					// the byte
          //根据偏移来算出该偏移所在的扇区

    int FileLength();			// Return the length of the file 
					// in bytes

    void Print();			// Print the contents of the file.

    bool ExtendFile(BitMap * freeMap , int bytes );

//-----------------------------------
//-----------------------------------
    void SetCreatTime();
    void SetLastVisitTime();
    void SetLastModifyTime();

    time_t GetCreatTime();
    time_t GetLastVistTime();
    time_t GetLastModifyTime();


public:
    time_t createTime;
    time_t lastVistTime;
    time_t lastModifyTime;
    int hdrSector;
//-------------------------------------
//-------------------------------------
  private:
    int numBytes;			// Number of bytes in the file文件具体大小
    int numSectors;			// Number of data sectors in the file文件所占扇区大小
    int dataSectors[NumDirect];		// Disk sector numbers for each data  block in the file文件具体所在的扇区
};

#endif // FILEHDR_H
