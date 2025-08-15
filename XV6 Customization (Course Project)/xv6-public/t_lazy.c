#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
int main(int argc, char *argv[])
{
    int pid=getpid();
    printf(1, "VAS: %d Pages\n", getvasize(pid) / PGSIZE);
    printf(1, "PAS: %d Pages\n", getpasize(pid));
    char *addr = (char *)mmap(3 * PGSIZE);
    printf(1, "------Mapping 3 Pages-----\n");
    printf(1, "VAS: %d Pages\n", getvasize(pid) / PGSIZE);
    printf(1, "PAS: %d Pages\n", getpasize(pid));
    for (int i = 0; i < 3; i++)
    {
    printf(1, "------Accessing Page %d-----\n",i, *(addr + i * PGSIZE ));
        printf(1, "VAS: %d Pages\n", getvasize(pid) / PGSIZE);
        printf(1, "PAS: %d Pages\n", getpasize(pid));
    }
    exit();
}
