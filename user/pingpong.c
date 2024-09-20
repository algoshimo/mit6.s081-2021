//子进程一个管道，父进程一个管道
//fd[0]:读 fd[1]:写 

# include"user/user.h"
# include"kernel/types.h"

int main(int argc , char*argv[])
{
    // int pipfd1[2];   //父进程向子进程发送数据
    // int pipfd2[2];   //子进程向父进程发送数据

    // pipe(pipfd1);
    // pipe(pipfd2);
    // // if( stauts == -1) {
    // //     exit(1);
    // // }

    // char s;
    // int pid = fork();
    // if(pid == 0) { //子进程
    //     //close(pipfd1[1]);
    //     while(read(pipfd1[0], &s, 1) == -1);
    //     printf("%d: received ping\n",getpid());
    //     write(pipfd2[1], &s, 1);
    // }
    // else { //父进程
    //     write(pipfd1[1], &s, 1);
    //     while(read(pipfd2[0], &s, 1) == -1);
    //     printf("%d: received pong\n",getpid());
    // }

    int pipfd1[2];   //父进程向子进程发送数据
    int pipfd2[2];   //子进程向父进程发送数据
    
    pipe(pipfd1);
    pipe(pipfd2);
    
    int pid = fork();
    char s;
    if(pid == 0) {
        close(pipfd1[1]);
        while(read(pipfd1[0],&s,1) == -1);
        printf("%d: received ping\n",getpid());
        write(pipfd2[1],&s,1);
        close(pipfd2[1]);
    }
    else {
        write(pipfd1[1],&s,1);
        while(read(pipfd2[0],&s,1) == -1);
        printf("%d: received pong\n",getpid());
    }
    exit(0);
}