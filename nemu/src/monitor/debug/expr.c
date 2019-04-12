#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ = 255, TK_DEC = 254, TK_HEX = 253,
  TK_REG = 252, TK_LBRACKET = 251, TK_RBRACKET = 250

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},              // spaces
  {"\\+", '+'},                   // plus
  {"==", TK_EQ},                  // equal
  {"[0-9]{1,9}", TK_DEC},         // dec
  {"[0-9a-fA-F]{1,8}", TK_HEX},   // hex
  {"\\$[eE]?[a-dA-D][xX]|\\$[eE]?[iI][pP]|\\$[a-dA-D][hHlL]|\\$[eE]?[dDsS][Ii]|\\$[eE]?[sSbB][pP]", TK_REG}, //reg
  {"\\(", TK_LBRACKET},           // left bracket
  {"\\)", TK_RBRACKET},           // right bracket
  {"-", '-'},                     // sub
  {"\\*", '*'},                   // mul
  {"/", '/'}                      // div
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		//scanf("nothing wrong here");

        switch (rules[i].token_type) {
			case TK_HEX:
				tokens[nr_token].type = TK_HEX;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				scanf("%s", tokens[nr_token].str);
				nr_token++;
			case TK_DEC:
				tokens[nr_token].type = TK_DEC;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				nr_token++;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  scanf("nothing ");

  return 0;
}
