#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
primes(int lpipe[2])
{
  close(lpipe[1]);
  int first, num;
  int n = read(lpipe[0], &first, sizeof(int));
  if(n != sizeof(int)) exit(0);
  printf("prime %d\n", first);
  int rpipe[2];
  pipe(rpipe);
  while(read(lpipe[0], &num, sizeof(int))){
    if(num % first != 0)
      write(rpipe[1], &num, sizeof(int));
  }
  close(lpipe[0]);
  close(rpipe[1]);
  if(fork() == 0){
    primes(rpipe);
  } else {
    close(rpipe[0]);
    wait(0);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  for (int i=2; i<=35; i++) {
    write(p[1], &i, sizeof(int));
  }
  if(fork()==0){
    primes(p);
  } else {
    close(p[0]);
    close(p[1]);
    wait(0);
  }
  exit(0);
}
