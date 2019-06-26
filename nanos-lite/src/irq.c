#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
extern _RegSet* schedule(_RegSet *prev);

static _RegSet* do_event(_Event e, _RegSet* r) {
  _RegSet *result = NULL;
  switch (e.event) {
	case _EVENT_SYSCALL: do_syscall(r);
						 break;
	case _EVENT_TRAP: result = schedule(r);
					  break;
	case _EVENT_IRQ_TIME: Log("IRQ_TIME!");
						  result = schedule(r);
						  break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return result;
}

void init_irq(void) {
  _asye_init(do_event);
}
