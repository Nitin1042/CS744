#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(worldpeace) //worldpeace
SYSCALL(numberofprocesses) //numpro
SYSCALL(spawn) //spawn
SYSCALL(getvasize) //vasize
SYSCALL(va_to_pa) //va2pa
SYSCALL(get_pgtb_size)
SYSCALL(get_user_pgtb_size)
SYSCALL(get_kernel_pgtb_size)
SYSCALL(getpasize)
SYSCALL(mmap) //mmap
