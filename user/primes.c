# include"user/user.h"
# include"kernel/types.h"

void solve(int pipfd[2])
{
    close(pipfd[1]);
    int num, prim;
    if(read(pipfd[0], &prim, sizeof(int)) <= 0) exit(1);
    printf("prime %d\n",prim);
    int new_pipfd[2];
    pipe(new_pipfd);

    if(fork() == 0) 
    {
        close(new_pipfd[1]);
        solve(new_pipfd);
    }
    else 
    {
        while(read(pipfd[0], &num, sizeof(int)) > 0)
        {
            if(num % prim !=0)
            {
                write(new_pipfd[1], &num, sizeof(int));
            }
        }
    }
}

// void solve(int pipfd[2])
// {
//     close(pipfd[1]);
//     int new_pipfd[2];
//     pipe(new_pipfd);

//     int prim;
//     int num;
//     if(read(pipfd[0],&prim,sizeof(int)) <= 0) exit(1);
//     printf("prime %d\n",prim);
//     while(read(pipfd[0],&num,sizeof(int)) > 0)
//     {
//         //printf("read : %d\n",num);
//         if(num % prim != 0) write(new_pipfd[1],&num,sizeof(int));
//     }
//     solve(new_pipfd);
// }

int main(int argc , char*argv[])
{   
    // int pipfd[2];
    // pipe(pipfd);
    // int pid;
    // pid = fork();
    // if( pid == 0)
    // {
    //     close(pipfd[1]);
    //     solve(pipfd);
    //     close(pipfd[0]);
    // }
    // else 
    // {
    //     for(int i=2;i<=35;i++) 
    //     {
    //         int num = i;
    //         close(pipfd[0]);
    //         write(pipfd[1], &num, sizeof(int));
    //     } 
    //     wait(&pid);
    // }
    
    
    int pipfd[2];
    pipe(pipfd);

    for(int i = 2;i<=35;i++)
    {
        int num = i;
        write(pipfd[1],&num,sizeof(int));
    }

    int pid = fork();
    
    if(pid == 0) 
    {
        close(pipfd[1]);
        solve(pipfd);
    }

    exit(1);
}
