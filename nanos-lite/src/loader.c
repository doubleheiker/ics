#include "common.h"
#include "memory.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);
#define DEFAULT_ENTRY ((void *)0x8048000)

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  int fd;//file descriptor
  fd = fs_open(filename, 0, 0);
  size_t filesize = fs_filesz(fd);
  void *pa;
  void *va;

  Log("loaded: [%d]%s size: %d", fd, filename, filesize);

  void *end = DEFAULT_ENTRY + filesize;
  for (va = DEFAULT_ENTRY; va < end; va += PGSIZE) {
	  pa = new_page();
	  Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
	  _map(as, va, pa);
	  fs_read(fd, pa, (end - va) < PGSIZE ? (end - va) : PGSIZE);
  }

  //fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
