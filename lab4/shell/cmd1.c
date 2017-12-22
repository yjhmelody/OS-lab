#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  char p[80];
  getcwd(p, 80);
  printf("%s\n", p);
}
