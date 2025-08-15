#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{

    printf(1, "Process size before mmap %d Bytes\n", getvasize(getpid()));
    char *addr = (char *)mmap(4096);
    printf(1, "Process size after mmap %d Bytes\n", getvasize(getpid()));
    printf(1, "Accessing a address allocated by mmap %p\n", (addr + 128), *(addr + 128));
    exit();
}
