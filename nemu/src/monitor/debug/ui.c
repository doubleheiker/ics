#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static struct {
  char *name;
  char *description;
  /*function point*/
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute N steps, 'si' without [N] will execute 'si 1' as defualt", cmd_si },
  { "info", "Print the registers' states, 'info r': print all registers' states", cmd_info },
  { "x", "Scan the memory, usage: x [N] [addr]", cmd_x },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
	char *arg = strtok(NULL, " ");
	/*Only input 'si'*/
	if (arg == NULL) {
		cpu_exec(1);
		return 0;
	}
	uint64_t n;
	sscanf(arg, "%llu", &n);
	cpu_exec(n);
	return 0;
}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	/*Only input 'info'*/
	if (arg == NULL) {
		printf("Please use 'help info' to see the usage of 'info'\n");
		return 0;
	}

	/*command 'r'*/
	if (strcmp(arg, "r") == 0) {
		/*print 32 bit register*/
		for (int i = 0; i < 8; ++i) {
			printf("%s:\t%8x\n", regsl[i], cpu.gpr[i]._32);
		}
		/*print 16 bit register*/
		for (int i = 0; i < 8; ++i) {
			printf("%s:\t%8x\n", regsw[i], cpu.gpr[i]._16);
		}
		/*print 8 bit register*/
		for (int i = 0; i < 4; ++i) {
			printf("%s:\t%8x\n", regsb[i], cpu.gpr[i]._8[0]);
			printf("%s:\t%8x\n", regsb[i+4], cpu.gpr[i]._8[1]); 
		}
	}

	/*command 'w'*/
	else if (strcmp(arg, "w") == 0){
		/*In PA1.3*/
	}

	else {
		printf("nemu: command not found: info %s\n", arg);
	}
	return 0;
}

static int cmd_x(char *args) {
	char *arg = strtok(NULL, " ");
	/*Only input x*/
	if (arg == NULL) {
		printf("Please use 'help x' to see the usage of 'x'\n");
		return 0;
	}
	
	/*ui*/
	printf("Address\tDword block...Byte sequence\n");

	/*n: The number of memory info */
	uint32_t n;
	sscanf(arg, "%u", &n);

	char *arg1 = strtok(NULL, " ");
	vaddr_t vaddress;
	sscanf(arg1, "%x", &vaddress);
	
	/*Print address*/
	for(int i = 0; i < n; i++) {
		printf("%x", vaddress);
		printf("%x", vaddr_read(vaddress, 4));
		vaddress += 4;
		printf("\n");
	}
	return 0;
}


void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
