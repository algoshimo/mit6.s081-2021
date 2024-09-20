# include "kernel/param.h"
# include "kernel/types.h"
# include "user/user.h"
int main(int argc , char *argv[])
{
    int buf_size = 0;
    char temp_buf[512];
    char *buf[MAXARG];

    while(1)
    {
        gets(temp_buf,512);
        if(temp_buf[0] == '\0')  break;
        temp_buf[strlen(temp_buf)-1] = '\0';
        buf[buf_size] = malloc(strlen(temp_buf) + 1);
        strcpy(buf[buf_size] ,  temp_buf);
        buf_size++;
    }

    // for(int i=0;i<buf_size;i++) 
    // {
    //     printf("%s\n",buf[i]);
    // }

    for(int i=0;i<buf_size;i++)
    {
        argv[argc+i] = buf[i];
    }

    for(int i=1;i<argc+buf_size ;i++)
    {
        argv[i-1] = argv[i];
    }
    argv[argc+buf_size-1] = '\0';
     exec(argv[0],argv);
    // for(int i=0;i<argc+buf_size;i++)
    // {
    //     printf("%d : %s\n",i,argv[i]);
    // }
    exit(0);
}