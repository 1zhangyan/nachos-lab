/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"
//#include <stdio.h>

void
testfunc()
{
    Open("testfile");
    Exit(0);
}
int
main()
{
    //Fork(testfunc);
Exit(0);
    /*Create("testfile");
    int id = Open("testfile");
    char * buffer = "0123456789012345678";
    Write(buffer, 10, id);
    Read(buffer, 10, Open("testfile"));
    */
    //char *buffer;
    //
    //Close(id);
    //Fork(void (*func)());
    //Yield();
    
    //Join(Exec("../test/sort"));
    //printf("This is userpro and get Open retvalue is %d\n" , a);
    //Fork(testfunc);
    //Exec("../test/sort");
    Halt();
    //Exit(0);
    /* not reached */
}
