#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2], p2[2];
  pipe(p1);
  pipe(p2);
  int pid = fork();
  if(pid == 0){
    close(p1[1]);
    char buf[1];
    read(p1[0], buf, sizeof(buf));
    printf("%d: received ping\n", getpid());
    close(p1[0]);

    close(p2[0]);
    write(p2[1], "o", 1);
    close(p2[1]);
  }else if(pid > 0){
    close(p1[0]);
    write(p1[1], "i", 1);
    close(p1[1]);
    wait((int*) 0);

    char buff[1];
    read(p2[0], buff, sizeof(buff));
    printf("%d: received pong\n", getpid());
    close(p2[0]);
  }else{
    fprintf(2, "fork error!\n");
    exit(1);
  }

  exit(0);
}
