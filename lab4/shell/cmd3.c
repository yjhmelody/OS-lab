#include <stdio.h>

int main(int num, char *s[]) {
  if (num != 1)
    printf("num is not 1\n");
  if (!remove(s[0]))
    printf("remove %s success\n", s[0]);
  else
    printf("error\n");
  return 0;
}
