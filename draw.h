#ifndef DRAW
#define DRAW

#include  "SDL.h"
#include    "parse.h"

#define STACKMAX 10
struct DrawInfo
{
  double x;
  double y;
  int  angle;
  double distance;
};

struct var_list
{
  struct var_list *next;
  char var;
  double  num;
};

struct scope
{
  struct var_list *var_table;
  struct DrawInfo *di;
};

struct scope_stack
{
  int sp;
  struct scope *stack[STACKMAX];
};

extern int start_draw(struct MAIN *);

#endif
