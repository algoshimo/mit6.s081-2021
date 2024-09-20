#include "user/user.h"
#include "kernel/types.h"

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(2, "Error: Missing argument");
        exit(1);
    }
    int time = atoi(argv[1]);
    if( sleep(time) < 0) {
        exit(1);
    }
    exit(0);
}