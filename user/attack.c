#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define SCAN_PAGES 128
#define PAGE_BYTES 4096
#define MAX_CANDIDATE 256

static int
alphanumeric(char c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'A' && c <= 'Z') ||
         (c >= 'a' && c <= 'z');
}

int
main(void)
{
  char *memory;
  int size = SCAN_PAGES * PAGE_BYTES;
  int offset = 0;

  memory = sbrk(size);
  if (memory == SBRK_ERROR)
    exit(1);

  while (offset < size) {
    int start;
    int length;
    char candidate[MAX_CANDIDATE];

    while (offset < size && !alphanumeric(memory[offset]))
      offset++;
    start = offset;
    while (offset < size && alphanumeric(memory[offset]))
      offset++;
    length = offset - start;
    if (length < 8 || length >= sizeof(candidate))
      continue;

    memmove(candidate, memory + start, length);
    candidate[length] = '\0';
    printf("%s\n", candidate);
  }
  exit(0);
}