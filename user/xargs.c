#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void
clear(char *xargv[MAXARG], int start){
  for(int i=start; i<MAXARG; i++){
    xargv[i] = 0;
  }
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xarg <cmd>\n");
    exit(1);
  }
  if(argc > MAXARG){
    fprintf(2, "xargs: too many arguments\n");
    exit(1);
  }
  char *xargv[MAXARG] = {0};
  int xargc = 0;
  for(; xargc+1 < argc; xargc++){
    xargv[xargc] = argv[xargc+1];
  }
  char buf[1024], c;
  int p = 0, start = 0;
  while(read(0, &c, 1)){
    switch (c){
    case ' ':
      buf[p] = 0;
      xargv[xargc++] = &buf[start];
      start = p;
      break;
    case '\n':
      buf[p] = 0;
      xargv[xargc++] = &buf[start];
      if(fork() == 0){
        exec(argv[1], xargv);
      } else {
        wait(0);
      }
      xargc = argc - 1;
      clear(xargv, xargc);
      p = 0;
      start = 0;
      break;
    default:
      buf[p++] = c;
      break;
    }
  }
  exit(0);
}
