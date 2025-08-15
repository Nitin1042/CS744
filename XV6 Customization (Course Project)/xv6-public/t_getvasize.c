#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int pid = getpid();
  printf(1, "Pid of the process is %d\n", pid);

  int process_size_before_sbrk = getvasize(getpid());
  printf(1, "Size of process:          %d Bytes\n", process_size_before_sbrk);
  printf(1, "Address returned by sbrk: 0x%x\n", sbrk(0));

  char *sbrk_after = sbrk(1024); // sbrk increase process by n bytes
  int process_size_after_sbrk = getvasize(getpid());
  printf(1, "Size of process:          %d Bytes\n", process_size_after_sbrk);
  printf(1, "Address returned by sbrk: x0%x\n", sbrk_after);

  exit();
}
