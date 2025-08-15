#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    printf(1, "Page Table Size %d Pages\n", get_pgtb_size());
    printf(1, "User Pages Table Size %d Pages\n", get_user_pgtb_size());
    printf(1, "kernel Pages Table Size %d Pages\n", get_kernel_pgtb_size());
    sbrk(4096 * 4096);
    printf(1,"------------Doing sbrk-----------\n");
    printf(1, "Page Table Size %d Pages\n", get_pgtb_size());
    printf(1, "User Pages Table Size %d Pages\n", get_user_pgtb_size());
    printf(1, "kernel Pages Table Size %d Pages\n", get_kernel_pgtb_size());
    exit();
}
