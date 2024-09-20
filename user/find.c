#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

static char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

void find(char *path,char *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
      
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
    
    if(fstat(fd, &st) < 0){ //获取文件状态信息(存在st结构体)
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
    //printf("%s : %d\n",path , st.type);
    //printf("%s\n",fmtname(path))
    switch (st.type)
    {
      
        case T_FILE :
            //strcpy(buf,fmtname(path));
            // printf("file is %s\n,want file %s\n",fmtname(path),filename);
            // printf("%d\n",buf , filename);
            if(strcmp(fmtname(path) , filename) == 0) 
            {
                //printf("%d\n",strcmp(filename , "README")); 
                printf("%s\n",path);
            }
            break;
        case T_DIR:
        
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            //printf("%s\n",buf);
            while(read(fd,&de,sizeof(de)) == sizeof(de)) {
                if(de.inum == 0)
                {
                  //printf("inum == 0\n"); 
                  continue;
                }

                //printf("de.name : %s\n",de.name);
                if(strcmp(de.name,".") == 0 || strcmp(de.name,"..") == 0) continue;
                
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                find(buf,filename);
            }
        break;
    
    }
    close(fd);
}
int main(int argc, char *argv[])
{
    find(argv[1],argv[2]);
    exit(0);
}