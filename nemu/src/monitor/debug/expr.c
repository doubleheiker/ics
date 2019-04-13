#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ = 257, TK_DEC = 258, TK_HEX = 259,
  TK_REG = 260, TK_LBRACKET = 261, TK_RBRACKET = 262

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
  {"0x[0-9a-fA-F]{1,8}", TK_HEX}, // hex
  {"[0-9]{1,9}", TK_DEC},         // dec
  {"\\$[eE]?[a-dA-D][xX]|\\$[eE]?[iI][pP]|\\$[a-dA-D][hHlL]|\\$[eE]?[dDsS][Ii]|\\$[eE]?[sSbB][pP]", TK_REG}, //reg
  {"\\(", TK_LBRACKET},           // left bracket
  {"\\)", TK_RBRACKET},           // right bracket
  {"-", '-'},                     // sub
  {"\\*", '*'},                   // mul
  {"/", '/'},                     // div
  /*{"!=", TK_NOEQ},*/                // not equal
  /*{"&&", TK_AND},*/                 // and
  /*{"\\|\\|", TK_OR},*/              // or
  /*{"!", TK_NO},*/                   // no
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
		//printf("enter make_Token\n");

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case TK_NOTYPE:
				break;
			case '+':
				tokens[nr_token].type = '+';
				nr_token++;
				//printf("%d", tokens[nr_token-1].type);
				break;
			case '-':
				tokens[nr_token].type = '-';
				nr_token++;
				break;
			case '*':
				tokens[nr_token].type = '*';
				nr_token++;
				break;
			case '/':
				tokens[nr_token].type = '/';
				nr_token++;
				break;
			case TK_EQ:
				tokens[nr_token].type = TK_EQ;
				nr_token++;
				break;
			case TK_HEX:
				tokens[nr_token].type = TK_HEX;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				nr_token++;
				break;
			case TK_DEC:
				tokens[nr_token].type = TK_DEC;
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				//printf("%s\n", tokens[nr_token].str);
				nr_token++;
				break;
			case TK_LBRACKET:
				tokens[nr_token].type = TK_LBRACKET;
				nr_token++;
				break;
			case TK_RBRACKET:
				tokens[nr_token].type = TK_RBRACKET;
				nr_token++;
				break;
         // default: TODO();
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

bool check_parentheses(int p, int q) {
	//printf("enter the check\n");
	if (tokens[p].type == TK_LBRACKET && tokens[q].type == TK_RBRACKET) {
		int i, lbras = 0;//lbras means the numbers of single left brackets
		i = p + 1;
		/*check the whole expression*/
		while (lbras >= 0 && i < q) {
			if (tokens[i].type == TK_LBRACKET) {
				lbras++;
			}
			else if (tokens[i].type == TK_RBRACKET) {
				lbras--;
			}
			i++;
		}
		if (lbras < 0) return false;
		else if (i == q && lbras != 0) return false;
		else return true;
	}
	else return false;
}

int find_dominated_op(int p, int q) {
	//printf("enter the dominated\n");
	int lbras = 0, op = -1;//op means the location of dominated op
	/*find the dominated op*/
	for (int i = p; i <= q; i++) {
		if (tokens[i].type == TK_LBRACKET) {
			lbras++;
		}
		else if (tokens[i].type == TK_RBRACKET) {
			lbras--;
		}
		if (lbras == 0) {
			switch(tokens[i].type) {
				case '+':
					op = i;
					break;
				case '-':
					op = i;
					break;
				case '*':
					op = i;
					break;
				case '/':
					op = i;
					break;
				case TK_EQ:
					op = i;
					break;
				default:
					break;
			}
		}
		/*if ')' number > '(', the expr is wrong*/
		else if (lbras < 0) {
			return -2;
		}
	}
	/*if lbras > 0, the expr is wrong*/
	if (lbras > 0) return -2;
	return op;
}

uint32_t eval(int p, int q) {
	//printf("enter eval\n");
	uint32_t res, val1, val2;
	if (p > q) {
		/*bad expr*/
		printf("Bad expression(1)!\n");
		assert(0);
	}
	else if (p == q) {
		/*single token, should be a number*/
		if (tokens[p].type == TK_DEC) {
			sscanf(tokens[p].str, "%d", &res);
			return res;
		}
		else if (tokens[p].type == TK_HEX) {
			sscanf(tokens[p].str, "%x", &res);
			return res;
		}
		else {
			printf("Bad expression(2)!\n");
			assert(0);
		}
	}
	else if (check_parentheses(p, q) == true) {
		/*the expr is surrounded by a matched pair of parentheses*/
		/*just throw away the parentheses*/
		return eval(p+1, q-1);
	}
	else {
		int op;
		op = find_dominated_op(p, q);
		/*Wrong expression*/
		if (op == -2) {
			printf("Bad expression(3)!\n");
			assert(0);
	 	}
		/*No dominated op*/
	 	else if (op == -1) {
			printf("Bad expression(4)!\n");
			assert(0);
		}
		val1 = eval(p, op - 1);
		val2 = eval(op + 1, q);
		switch (tokens[op].type) {
			case '+':
				return val1 + val2;
			case '-':
				return val1 - val2;
			case '*':
				return val1 * val2;
			case '/':
				return val1 / val2;
			case TK_EQ:
				return (val1 == val2);
			default:
				assert(0);
	 	}
	} 
}

uint32_t expr(char *e, bool *success) {
  //printf("entered expr\n");
  if (!make_token(e)) {
	//printf("if true\n");
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  printf("nr_token: %d\n", nr_token);
  *success = true;
  return eval(0, nr_token - 1);
}
