#include  <unistd.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <errno.h>

#include    "parse.h"


struct MAIN *read_MAIN(void);
struct INSTRCTLST *read_INSTRCTLST(void);
struct INSTRUCTION *read_INSTRUCTION(void);
struct FD *read_FD(void);
struct LT *read_LT(void);
struct RT *read_RT(void);
struct VARNUM *read_VARNUM(void);
struct DO *read_DO(void);
struct SET *read_SET(void);
char read_VAR(void);
struct POLISH *read_POLISH(void);

int is_op(char c)
{
  /*printf("c is %c\n", c);*/
  if(c == '+' || c == '-' || c == '*' || c == '/')
    return 1;
  else 
    return 0;
}
int is_number(const char *s)
{
  int len = strlen(s);
  for(int i = 0; i < len; i++)
  {
    if(s[i] != '.' && (s[i] < '0' || s[i] > '9'))
      return 0;
  }
  return 1;
}
void syntax_error(const char *s, const char *keyword)
{
  fprintf(stderr, "%s: %s\n", s, keyword);
  _exit(-1);
}

///////////////////////////////////

struct MAIN *read_MAIN()
{
  char word[VARMAXLEN];

  struct MAIN *rv = (struct MAIN *)malloc(sizeof(struct MAIN));
  rv->next = NULL;

  ///////////
  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "MAIN");

  if(strcmp(word, "{") == 0)
  {
    rv->next = read_INSTRCTLST();
  }
  else
  {
    syntax_error("no '{' at the beginning", "MAIN");
  }
  return rv;
}

struct INSTRCTLST *read_INSTRCTLST()
{
  char word[VARMAXLEN];

  struct INSTRCTLST *rv = NULL;

  /////////////
  long old_offset = ftell(stdin);

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "INSTRCTLST");

  if(strcmp(word, "}") == 0)
  {
    return rv;
  }
  else
  {
    fseek(stdin, old_offset, SEEK_SET);

    rv = (struct INSTRCTLST*)malloc(sizeof(struct INSTRCTLST));
    rv->next = NULL;
    rv->ins = NULL;

    rv->ins = read_INSTRUCTION();
    rv->next = read_INSTRCTLST();
  }
  return rv;
}

struct INSTRUCTION *read_INSTRUCTION()
{
  char word[VARMAXLEN];

  struct INSTRUCTION *rv = (struct INSTRUCTION*)malloc(sizeof(struct INSTRUCTION));

  ///////////
  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "INSTRUCTION");

  if(strcmp(word, "FD") == 0)
  {
    rv->type = IT_FD;
    rv->ins.fd = read_FD();
  }
  else if(strcmp(word, "LT") == 0)
  {
    rv->type = IT_LT;
    rv->ins.lt = read_LT();
  }
  else if(strcmp(word, "RT") == 0)
  {
    rv->type = IT_RT;
    rv->ins.rt = read_RT();
  }
  else if(strcmp(word, "DO") == 0)
  {
    rv->type = IT_DO;
    rv->ins.doloop = read_DO();
  }
  else if(strcmp(word, "SET") == 0)
  {
    rv->type = IT_SET;
    rv->ins.set = read_SET();
  }
  else 
  {
    syntax_error("unknown instruction", word);
  }
  return rv;
}

struct FD *read_FD()
{
  char word[VARMAXLEN];

  struct FD *rv = (struct FD*)malloc(sizeof(struct FD));

  //////////////
  long old_offset = ftell(stdin);

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "FD");

  fseek(stdin, old_offset, SEEK_SET);

  rv->num = read_VARNUM();
  return rv;
}

struct LT *read_LT()
{
  char word[VARMAXLEN];

  struct LT *rv = (struct LT*)malloc(sizeof(struct LT));

  ////////////////
  long old_offset = ftell(stdin);

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "LT");

  fseek(stdin, old_offset, SEEK_SET);

  rv->num = read_VARNUM();

  return rv;
}

struct RT *read_RT()
{
  char word[VARMAXLEN];

  struct RT *rv = (struct RT*)malloc(sizeof(struct RT));

  /////////////////
  long old_offset = ftell(stdin);

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "RT");

  fseek(stdin, old_offset, SEEK_SET);

  rv->num = read_VARNUM();

  return rv;
}

struct DO *read_DO()
{
  char word[VARMAXLEN];

  struct DO *rv = (struct DO*)malloc(sizeof(struct DO));

  ////////////////
  rv->var = read_VAR();

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "DO");

  if(strcmp(word, "FROM") != 0)
    syntax_error("miss FROM", "DO");

  rv->from = read_VARNUM();

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "DO");

  if(strcmp(word, "TO") != 0)
    syntax_error("miss TO", "DO");

  rv->to = read_VARNUM();

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "DO");

  if(strcmp(word, "{") != 0)
    syntax_error("miss {", "DO");

  rv->il = read_INSTRCTLST();

  return rv;
}

struct SET *read_SET()
{
  char word[VARMAXLEN];

  struct SET *rv = (struct SET *)malloc(sizeof(struct SET));

  ////////////
  rv->var = read_VAR();

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "SET");

  if(strcmp(word, ":=") != 0)
    syntax_error("miss := ", "SET");

  rv->po = read_POLISH();

  return rv;
}

struct POLISH *read_POLISH()
{
  char word[VARMAXLEN];

  struct POLISH *rv = NULL;

  /////////////
  long old_offset = ftell(stdin);

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "POLISH");

  if(strcmp(word, ";") == 0)
  {
    return rv;
  }
  else if(strlen(word) == 1 && is_op(word[0]))
  {
    /*printf("here is op\n");*/
    rv = (struct POLISH*)malloc(sizeof(struct POLISH));

    rv->type = PO_OP;
    rv->po.op = word[0];

    rv->next = read_POLISH();
  }
  else
  {
    /*printf("here is varnum\n");*/
    fseek(stdin, old_offset, SEEK_SET);

    rv = (struct POLISH*)malloc(sizeof(struct POLISH));
    
    rv->type = PO_VN;
    rv->po.vn = read_VARNUM();

    rv->next = read_POLISH();
  }
  return rv;

}

char read_VAR()
{
  char word[VARMAXLEN];

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "VAR");

  if(strlen(word) != 1 || word[0] < 'A' || word[0] > 'Z')
    syntax_error("a bad name", "VAR");

  return word[0];
}

struct VARNUM *read_VARNUM()
{
  char word[VARMAXLEN];

  struct VARNUM *rv = (struct VARNUM *)malloc(sizeof(struct VARNUM));

  if(scanf("%s", word) == EOF)
    syntax_error("unexpected end", "VARNUM");

  if(!is_number(word)) 
  {
    if(strlen(word) == 1 && word[0] >= 'A' && word[0] <= 'Z')
    {
      rv->type = VN_VAR;
      rv->vn.var = word[0];
    }
    else
      syntax_error("is not a number", word);
  }
  else
  {
    rv->type = VN_NUM;
    rv->vn.num = atof(word);
  }

  return rv;
}
