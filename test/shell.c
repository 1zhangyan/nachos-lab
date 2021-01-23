#include "syscall.h"

int CompareString(char *s1 , char *s2 , int n)
{
    int i = 0;
    for(i = 0 ; i <= n ; i ++)
    {
        if (s1[i] != s2[i])
        return 0;
    }
    return 1;
} 


int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;
    int tmp;

    prompt[0] = '-';
    prompt[1] = '$';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	
	do {
	
	    Read(&buffer[i], 1, input); 
        //tmp = (buffer[i]);

	} while( buffer[i++] != '\n' );

	buffer[--i] = '\0';
//=======================ls==============
    if(buffer[0]=='l'&&buffer[1]=='s')
    {
        Ls();
    }
//===========pwd============
    if(buffer[0]=='p'&&buffer[1]=='w'&&buffer[2]=='d')
    {
        Pwd();
    }
//================quit====
    if(buffer[0]=='q'&&buffer[1]=='u'&&buffer[2]=='i'&&buffer[3]=='t')
    {
        Halt();
    }
    ///=============./=========
	if( buffer[0]=='.'&&buffer[1]=='/') {
		newProc = Exec(buffer + 2);
		Join(newProc);
	}
    //============cd ===============
    if(buffer[0]=='c'&&buffer[1]=='d'&&buffer[2]==' ')
    {
        Cd(buffer+3);
    }
    //=========================== mkdir==================
     if(buffer[0]=='m'&&buffer[1]=='k'&&buffer[2]=='d'&&buffer[3]=='i'&&buffer[4]=='r'&&buffer[5]==' ')
    {
        Mkdir(buffer+6);
    }

    //====================rmf============
    if(buffer[0]=='r'&&buffer[1]=='m'&&buffer[2]=='f'&&buffer[3]==' ')
    {
        Rmf(buffer+4);
    }
    //==================rmd==============
    if(buffer[0]=='r'&&buffer[1]=='m'&&buffer[2]=='d'&&buffer[3]==' ')
    {
        Rmd(buffer+4);
    }

     if(buffer[0]=='t'&&buffer[1]=='o'&&buffer[2]=='u'&&buffer[3]=='c'&&buffer[4]=='h'&&buffer[5]==' ')
    {
        Touch(buffer+6);
    }

    }
}

