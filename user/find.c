#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

static int
name_matches(const char *entry, const char *target)
{
  int i;

  for (i = 0; i < DIRSIZ; i++) {
    if (target[i] == '\0')
      return entry[i] == '\0' || entry[i] == ' ';
    if (entry[i] != target[i])
      return 0;
  }
  return target[i] == '\0';
}

static int
path_matches(const char *path, const char *target)
{
  const char *name = path;

  for (const char *p = path; *p; p++) {
    if (*p == '/')
      name = p + 1;
  }
  return strcmp(name, target) == 0;
}

static void
report_match(char *path, char **command, int command_count)
{
  int pid;

  if (command_count == 0) {
    printf("%s\n", path);
    return;
  }

  char *args[MAXARG];
  for (int i = 0; i < command_count; i++)
    args[i] = command[i];
  args[command_count] = path;
  args[command_count + 1] = 0;

  pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }
  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }
  wait(0);
}

static void
find_path(char *path, const char *target, char **command, int command_count)
{
  int fd;
  struct stat st;

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    if (path_matches(path, target))
      report_match(path, command, command_count);
    close(fd);
    return;
  }

  struct dirent entry;
  char child[MAXPATH];
  int path_len = strlen(path);
  if (path_len + 1 + DIRSIZ + 1 > sizeof(child)) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  memmove(child, path, path_len);
  child[path_len] = '/';
  while (read(fd, &entry, sizeof(entry)) == sizeof(entry)) {
    if (entry.inum == 0 || name_matches(entry.name, ".") ||
        name_matches(entry.name, ".."))
      continue;

    memmove(child + path_len + 1, entry.name, DIRSIZ);
    child[path_len + 1 + DIRSIZ] = '\0';
    if (stat(child, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", child);
      continue;
    }

    if (name_matches(entry.name, target))
      report_match(child, command, command_count);
    if (st.type == T_DIR)
      find_path(child, target, command, command_count);
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char **command = 0;
  int command_count = 0;

  if (argc < 3 || argc > MAXARG) {
    fprintf(2, "usage: find path name [-exec command [args...]]\n");
    exit(1);
  }
  if (argc > 3) {
    if (strcmp(argv[3], "-exec") != 0 || argc < 5 || argc > MAXARG - 1) {
      fprintf(2, "usage: find path name [-exec command [args...]]\n");
      exit(1);
    }
    command = &argv[4];
    command_count = argc - 4;
  }

  find_path(argv[1], argv[2], command, command_count);
  exit(0);
}