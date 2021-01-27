// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    SetCreatTime();
    SetLastVisitTime();
    SetLastModifyTime();

    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
	return FALSE;		// not enough space


    if (numSectors <= NumDirect)
    {
        int freeSpace = freeMap->FirstFind(numSectors); 
        printf("freespace = %d =====================\n"  , freeSpace);
        if(freeSpace != -1)
        {
            for(int  i = 0 ;  i < numSectors ; i++)
                dataSectors[i] = freeSpace + i;
        }
        else
        {
            for(int  i = 0  ; i < numSectors ; i++)
            dataSectors[i] = freeMap->Find();
        }

        return TRUE;
    }
    else
    {
        
        for (int i = 0; i < NumDirect - 1; i++)
	        dataSectors[i] = freeMap->Find();
        dataSectors[NumDirect-1] = freeMap->Find();
        int secondIndex[32];
        for(int  i = 0  ; i < numSectors - NumDirect + 1 ; i++)
            secondIndex[i] = freeMap->Find();
        synchDisk->WriteSector(dataSectors[NumDirect-1],(char *)secondIndex);
        return TRUE;

    }
    
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(BitMap *freeMap)
{
    if (numSectors <= NumDirect)
        for (int i = 0; i < numSectors; i++) 
        {
            ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
            freeMap->Clear((int) dataSectors[i]);
        }
    else
    {
        char *secondIndex = new char[SectorSize];
        synchDisk->ReadSector(dataSectors[NumDirect-1],secondIndex);
        
        for(int i = 0 ; i <numSectors- NumDirect + 1 ; i++ )
            freeMap->Clear((int) secondIndex[i * 4]);
        for(int i = 0 ; i < NumDirect ;i++)
            freeMap->Clear((int) dataSectors[i]);
    }
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
    // hdrSector = sector;
    SetLastVisitTime();
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    SetLastModifyTime();
    hdrSector = sector;
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    if (offset < SectorSize*(NumDirect-1))
        return(dataSectors[offset / SectorSize]);
    else
    {
        int secondIndexPosition = (offset - (SectorSize*(NumDirect - 1)))/SectorSize;
        char * secondIndex = new char[SectorSize];
        synchDisk->ReadSector(dataSectors[NumDirect-1],secondIndex);
        return  int(secondIndex[secondIndexPosition * 4]);
    }
    
    
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------
void
FileHeader::Print()
{

    printf("===========================\n");
    printf("numDirect :%d \n" , NumDirect);
    printf("===========================\n");
    int i, j, k;
    char *data = new char[SectorSize];
    struct tm * localCreateTime;
    struct tm * localVisitTime;
    struct tm * localModifyTime;
    localCreateTime = localtime(&createTime);
    localVisitTime =  localtime(&lastVistTime);
    localModifyTime = localtime(&lastModifyTime);
    
    printf("FileHeader contents.  File size: %d.\n", numBytes);
    printf("CreateTime : %d/%d/%d %d:%d:%d\n",localCreateTime->tm_year+1900,(localCreateTime->tm_mon+1)%13,localCreateTime->tm_mday,(localCreateTime->tm_hour+8)%24,localCreateTime->tm_min,localCreateTime->tm_sec);
    printf("LastVisitTime : %d/%d/%d %d:%d:%d\n",localVisitTime->tm_year+1900,(localVisitTime->tm_mon+1)%13,localVisitTime->tm_mday,(localVisitTime->tm_hour+8)%24,localVisitTime->tm_min,localVisitTime->tm_sec);
    printf("LastModifyTime : %d/%d/%d %d:%d:%d\n",localModifyTime->tm_year+1900,(localModifyTime->tm_mon+1)%13,localModifyTime->tm_mday,(localModifyTime->tm_hour+8)%24,localModifyTime->tm_min,localModifyTime->tm_sec);   
    printf("File blocks:\n");

    if (numSectors <= (NumDirect-1))
        for (i = 0; i < numSectors; i++)
            printf("%d ", dataSectors[i]);
    else
    {
        printf("Direct index :");
        for(i = 0 ; i < NumDirect -1 ; i ++)
            printf("%d ",dataSectors[i]);
        printf("\nSecond index Block : %d \n" , dataSectors[NumDirect-1]);
        printf("Second index :");
        char *secondIndex = new char[SectorSize];
        synchDisk->ReadSector(dataSectors[NumDirect-1],secondIndex);
        //int j = 0;
        for(int i = 0 ; i < numSectors - NumDirect + 1 ; i++)
        {
            printf("%d " , int(secondIndex[i*4]));
            //j = j + 4;
        }
    }
    printf("\nFile contents:\n");

    if (numSectors <= (NumDirect-1))
    {
        for (i = k = 0; i < numSectors; i++) {
        synchDisk->ReadSector(dataSectors[i], data);
            for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
            if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
            printf("%c", data[j]);
                else
            printf("\\%x", (unsigned char)data[j]);
        }
            printf("\n"); 
        }
    }
    else
    {
        for (i = k = 0; i < NumDirect-1; i++) 
        {
            printf("Sector:%d\n" , dataSectors[i]);
            synchDisk->ReadSector(dataSectors[i], data);
                for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) 
                {
                    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                        printf("%c", data[j]);
                    else
                        printf("\\%x", (unsigned char)data[j]);
                }
                printf("\n"); 
        }
        char *secondIndex = new char[SectorSize];
        synchDisk->ReadSector(dataSectors[NumDirect-1],secondIndex);
        for(i = 0 ; i < numSectors-(NumDirect-1) ;i++)
        {
            printf("Sector:%d\n" , int(secondIndex[i*4]));
            synchDisk->ReadSector(int(secondIndex[i*4]),data);
            for(j = 0 ;(j < SectorSize) && (k < numBytes); j++, k++ )
            {
                if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                    printf("%c", data[j]);
                else
                    printf("\\%x", (unsigned char)data[j]);
            }
            printf("\n");
        }
    }
    
    delete [] data;
}


//-----------------
//
//----------------
bool
FileHeader::ExtendFile(BitMap *freeMap , int bytes)
{
    int newFileLength = numBytes + bytes;
    int preSectorNum = numSectors;
    int newNumSectors = divRoundUp(newFileLength,SectorSize);
    if(newNumSectors == preSectorNum)
    {
        numBytes = newFileLength;
        return true;
    }
    if (freeMap->NumClear() < newNumSectors - preSectorNum)
        return false;
    printf("\nNeed Extend %d Sectors" , newNumSectors - preSectorNum);
    printf("New Allocate Sectors Index :");
    for (int i = preSectorNum; i < newNumSectors ; i++)
    {
        int temp = freeMap->Find();
        dataSectors[i] = temp;
        printf(" %d\n", temp);
        numBytes = newFileLength;
        numSectors = newNumSectors;
        return true;
    }
}

//------fileAddOperation
void
FileHeader::SetCreatTime()
{
   createTime = time(NULL);
}
void
FileHeader::SetLastVisitTime()
{
    lastVistTime  = time(NULL);
}
void 
FileHeader::SetLastModifyTime()
{
    lastModifyTime = time(NULL);
}

time_t 
FileHeader::GetCreatTime()
{
    return createTime;
}
time_t 
FileHeader::GetLastVistTime()
{
    return lastVistTime;
}
time_t 
FileHeader::GetLastModifyTime()
{
    return lastModifyTime;
}
