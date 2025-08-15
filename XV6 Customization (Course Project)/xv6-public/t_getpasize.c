#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int process_size_before_sbrk = getpasize(getpid());
    printf(1, "Virtual Address Space of process: %d Pages\n", (uint)getvasize(getpid()) / 4096);
    printf(1, "Number of physical pages allocated to process: %d Pages\n", process_size_before_sbrk);
    sbrk(11 * 4096); // Allocating 10 Physical Pages to process
    int process_size_after_sbrk = getpasize(getpid());
    printf(1, "Virtual Address Space of process after sbrk: %d Pages\n", (uint)getvasize(getpid()) / 4096);
    printf(1, "Number of physical pages allocated to process after sbrk: %d Pages\n", process_size_after_sbrk);
    exit();
}
