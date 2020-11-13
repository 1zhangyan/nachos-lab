/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */


#include "syscall.h"
//#include "copyright.h"
//#include "system.h"
//#include "syscall.h"
//#include "../userprog/machine.h"

#define ARRLEN 5

int A[ARRLEN];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
    
    int i, j, tmp;
   // printf("ENTER TEST\n");
    // first initialize the array, in reverse sorted order 
    for (i = 0; i < ARRLEN; i++)		
        A[i] = ARRLEN - i;

    // then sort! 
    for (i = 0; i < (ARRLEN-1); i++)   
        for (j = i; j < ((ARRLEN-1) - i); j++)
	   if (A[j] > A[j + 1]) {	// out of order -> need to swap ! 
	      tmp = A[j];
	      A[j] = A[j + 1];
	      A[j + 1] = tmp;
    	   }
    //Exit(A[0]);		// and then we're done -- should be 0!     
    //interrupt->Exit_user_pro();
    Halt();
    //return 0;
    //USER_PRO_EXIT();
    
    

}
