#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int N = 10;
  int va = (int)&N; // the virtual address of n
  int id = fork();
  if (id < 0)
    exit();
  else if (id == 0)
  {
    // In child we want to see the virtual and physical address of n
    printf(1, "Virtual address of Var N in child    %p\n", va);
    printf(1, "Physical address on Var N in child   %p\n", va_to_pa(va));
  }
  else
  {
    // In parent we want to see the virtual and physical address of n
    wait();
    printf(1, "Virtual address of Var N in parent   %p\n", va);
    printf(1, "Physical address on Var N in parent  %p\n", va_to_pa(va));
  }
  exit();
}
