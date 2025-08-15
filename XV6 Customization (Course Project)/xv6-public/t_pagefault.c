#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int pid=getpid();
  printf(1,"Pid of the process is %d\n",pid);
  char *sbrk_before=sbrk(0);
  printf(1,"Simulating a page_fault on addr %p\n",(sbrk_before+4096));
  int n=*(sbrk_before+4096); // why we are doing this any guesses??
  printf(1,"%d\n",n);
  exit();
}
