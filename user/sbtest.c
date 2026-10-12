#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static void
report(char *what, int fd)
{
  if (fd < 0) {
    printf("sbtest: forkpath child %s denied\n", what);
  } else {
    printf("sbtest: forkpath child %s ok\n", what);
    close(fd);
  }
}

int
main(int argc, char *argv[])
{
  int result;

  if (argc != 2) {
    fprintf(2, "Usage: sbtest getpid|fork|escape|forkpath\n");
    exit(1);
  }

  if (strcmp(argv[1], "getpid") == 0) {
    printf("sbtest: getpid ret %d\n", getpid());
  } else if (strcmp(argv[1], "fork") == 0) {
    result = fork();
    if (result == 0) {
      printf("sbtest: CHILD RAN\n");
      exit(0);
    }
    if (result > 0)
      wait(0);
    printf("sbtest: fork ret %d\n", result);
  } else if (strcmp(argv[1], "escape") == 0) {
    interpose(0, 0);
    interpose(0, "README");
    result = open("README", O_RDONLY);
    if (result < 0) {
      printf("sbtest: escape confined\n");
    } else {
      printf("sbtest: escape ESCAPED\n");
      close(result);
    }
  } else if (strcmp(argv[1], "forkpath") == 0) {
    result = fork();
    if (result == 0) {
      report("README", open("README", O_RDONLY));
      report("exec.sh", open("exec.sh", O_RDONLY));
      exit(0);
    }
    if (result > 0)
      wait(0);
  } else {
    fprintf(2, "sbtest: unknown test %s\n", argv[1]);
    exit(1);
  }

  printf("sbtest: alive\n");
  exit(0);
}