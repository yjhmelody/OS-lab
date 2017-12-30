#include <stdio.h>

int getId(char s[]) {
  static char dict[][22] = {"exit", "cwd", "clear", "rm"};
  for (int i = 0; i < 4; ++i)
    if (strcmp(dict[i], s) == 0)
      return i;
  return -1;
}

void show() { printf(">"); }

int main() {
  char s[111];
  show();
  while (~scanf("%s", s)) {
    int id = getId(s);
    if (id == 0)
      exit(0);
    else if (id == 1) {
      int p = fork();
      if (p == 0)
        execl("./cmd1", NULL);
      else
        wait(NULL);
    } else if (id == 2) {
      int p = fork();
      if (p == 0)
        execl("./cmd2", NULL);
      else
        wait(NULL);
    } else if (id == 3) {
      char arg[55];
      scanf("%s", arg);
      int p = fork();
      if (p == 0)
        execl("./cmd3", arg, NULL);
      else
        wait(NULL);
    } else {
      printf("Command not found\n");
    }
    show();
  }
  return 0;
}
