#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
#define DATAREG 0x60    // Keyboard data reg
#define FLAGREG 0x64    // Keyboard flag reg
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return (inl(RTC_PORT)-boot_time);
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
/*
  int len = sizeof(uint32_t) * ( (x + w >= _screen.width) ? _screen.width - x : w );
  uint32_t *p_fb = &fb[y * _screen.width + x];
  for (int j = 0; j < h; j ++) {
    if (y + j < _screen.height) {
      memcpy(p_fb, pixels, len);
    }
    else {
      break;
    }
    p_fb += _screen.width;
    pixels += w;
  }
*/
  int col, row;
  for(row = y; row < y+h; row++) {
	  for(col = x; col < x+w; col++)
		  fb[col + row*_screen.width] = pixels[(row-y)*w+(col-x)];
  }
}

void _draw_sync() {
}

int _read_key() {
  uint32_t keys = _KEY_NONE;
  if (inb(FLAGREG) & 1) {
	  keys = inl(DATAREG);
  }
  return keys;
}
