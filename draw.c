#include  <stdlib.h>
#include  <math.h>
#include  "draw.h"

//always add var in the top scope
static void add_var(struct scope_stack *ss, char var, double num)
{
  struct scope *s = ss->stack[ss->sp];
  struct var_list *vl;

  if(s->var_table == NULL)
  {
    s->var_table =  (struct var_list*)malloc(sizeof(struct var_list));
    vl = s->var_table;
  }
  else
  {
    vl = s->var_table;
    while(1) 
    {
      if(vl->var == var)
      {
	///already existed
	vl->num = num;
	return;
      }
      if(vl->next == NULL)
	break;
      else
	vl = vl->next;
    }
    vl->next = (struct var_list*)malloc(sizeof(struct var_list));
    vl = vl->next;
  }

  vl->next = NULL;
  vl->var = var;
  vl->num = num;

  return;
}

double get_num(struct scope_stack *ss, struct VARNUM *vn)
{
  if(vn->type == VN_NUM)
    return vn->vn.num;
  else
  {
    for(int i = ss->sp; i >= 0; i--)
    {
      struct var_list *vl = ss->stack[i]->var_table;
      while(vl != NULL)
      {
	if(vl->var == vn->vn.var)
	  return vl->num;
	vl = vl->next;
      }
    }
  }
  fprintf(stderr, "no VAR found\n");
  exit(-2);
}

double calc_polish(struct scope_stack *ss, struct POLISH *po)
{
  double calc_stack[100];
  int sp = -1;

  struct POLISH *next = po;
  if(next == NULL) return 0;

  while(next != NULL)
  {
    if(next->type == PO_VN)
    {
      calc_stack[++sp] = (double)get_num(ss, next->po.vn);
    }
    else
    {
      if(sp < 1) 
      {
	fprintf(stderr, "POLISH is invalid\n");
	exit(-2);
      }
      else
      {
	char op = next->po.op;
	switch(op)
	{
	  case '+':
	    sp--;
	    calc_stack[sp] = calc_stack[sp] + calc_stack[sp+1];
	    break;
	  case '-':
	    sp--;
	    calc_stack[sp] = calc_stack[sp] - calc_stack[sp+1];
	    break;
	  case '*':
	    sp--;
	    calc_stack[sp] = calc_stack[sp] * calc_stack[sp+1];
	    break;
	  case '/':
	    sp--;
	    if(calc_stack[sp+1] == 0)
	    {
	      fprintf(stderr, "POLISH: divide 0\n");
	      exit(-2);
	    }
	    calc_stack[sp] = calc_stack[sp] / calc_stack[sp+1];
	    break;
	}
      }
    }
    next = next->next;
  }
  if(sp != 0) 
  {
    fprintf(stderr, "POLISH is invalid\n");
    exit(-2);
  }

  return calc_stack[0];

}

/////////////////////////////////////////////
static void DrawPixel(SDL_Surface *screen, Uint16 x, Uint16 y)
{
  Uint8 R = 255;
  Uint8 G = 255;
  Uint8 B = 255;
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);

  if ( SDL_MUSTLOCK(screen) ) {
    if ( SDL_LockSurface(screen) < 0 ) {
      return;
    }
  }
  switch (screen->format->BytesPerPixel) {
    case 1: { /* Assuming 8-bpp */
	      Uint8 *bufp;

	      bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
	      *bufp = color;
	    }
	    break;

    case 2: { /* Probably 15-bpp or 16-bpp */
	      Uint16 *bufp;

	      bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
	      *bufp = color;
	    }
	    break;

    case 3: { /* Slow 24-bpp mode, usually not used */
	      Uint8 *bufp;

	      bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
	      *(bufp+screen->format->Rshift/8) = R;
	      *(bufp+screen->format->Gshift/8) = G;
	      *(bufp+screen->format->Bshift/8) = B;
	    }
	    break;

    case 4: { /* Probably 32-bpp */
	      Uint32 *bufp;

	      bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
	      *bufp = color;
	    }
	    break;
  }
  if ( SDL_MUSTLOCK(screen) ) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateRect(screen, x, y, 1, 1);
}

static void DrawLine(SDL_Surface *screen, Uint16 x1, Uint16 y1, Uint16 x2, Uint16 y2)
{
  double x = x2 - x1;
  double y = y2 - y1;
  double length = sqrt( x*x + y*y);

  double addx = x/length;
  double addy = y/length;

  x = x1;
  y = y1;

  for(double i = 0; i < length; i += 1)
  {
    DrawPixel(screen, (int)x, (int)y);
    x += addx;
    y += addy;
  }
}

static void DrawTurtle(SDL_Surface *screen, struct DrawInfo *di)
{
  if(di->distance == 0) return;

  double x2, y2;
  x2 = di->x + di->distance * cos((double)di->angle/360*2*3.1415);
  y2 = di->y - di->distance * sin((double)di->angle/360*2*3.1415);

  DrawLine(screen, di->x, di->y, x2, y2);
  //printf("(%d %d)\n", x2, y2);

  di->x = x2;
  di->y = y2;

  return;
}

static int draw(SDL_Surface *, struct MAIN *);

int start_draw(struct MAIN *m)
{
  if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "error: %s", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
  //////////////

  SDL_Surface *screen;
  screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);

  draw(screen, m);

  //////////////
  SDL_Event event;
  int quit = 0;
  while(!quit)
  {
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
	case SDL_KEYUP:
	  quit = 1;
	  break;
      }
    }
  }
  return 0;
}

static int draw_INSTRUCTION(SDL_Surface *, struct INSTRUCTION *, struct scope_stack *);

static int draw(SDL_Surface *screen, struct MAIN *m)
{
  //start point
  Uint16 x0 = 640/2;
  Uint16 y0 = 480/2;

  //the top level
  struct DrawInfo *di = (struct DrawInfo *)malloc(sizeof(struct DrawInfo));
  di->x = x0;
  di->y = y0;
  di->angle = 0;
  di->distance = 0;

  //init scope stack
  struct scope_stack *ss = (struct scope_stack *)malloc(sizeof(struct scope_stack));
  ss->sp = 0;
  ss->stack[0] = (struct scope *)malloc(sizeof(struct scope));
  ss->stack[0]->var_table = NULL;
  ss->stack[0]->di = di;
  /////////////////

  struct INSTRCTLST *next = m->next;

  while(next != NULL)
  {
    draw_INSTRUCTION(screen, next->ins, ss);
    next = next->next;
  }

  return 0;
}

static int draw_INSTRUCTION(SDL_Surface *screen, struct INSTRUCTION *ins, struct scope_stack *ss)
{
  struct DrawInfo *di = ss->stack[ss->sp]->di;
  /*printf("TYPE%d\n", ins->type);*/
  switch(ins->type){
    case IT_FD:
      di->distance = get_num(ss, ins->ins.fd->num);
      /*printf("FD%f\n", di->distance);*/
      DrawTurtle(screen, di);
      di->distance = 0;
      break;
    case IT_LT:
      di->angle = (di->angle + 
	  get_num(ss, ins->ins.lt->num) + 360);
      di->angle = di->angle - ((int)((int)di->angle/360))*360;
      /*printf("LT%d\n", di->angle);*/
      break;
    case IT_RT:
      di->angle = (di->angle - 
	  get_num(ss, ins->ins.rt->num) + 360);
      di->angle = di->angle - ((int)((int)di->angle/360))*360;
      /*printf("RT%d\n", di->angle);*/
      break;
    case IT_SET:
      add_var(ss, ins->ins.set->var, 
	  calc_polish(ss, ins->ins.set->po));
      break;
    case IT_DO:
      {
	int from = get_num(ss, ins->ins.doloop->from);
	int to = get_num(ss, ins->ins.doloop->to);

	//copy to a new DrawInfo
	di = (struct DrawInfo *)malloc(sizeof(struct DrawInfo));
	di->x = ss->stack[ss->sp]->di->x;
	di->y = ss->stack[ss->sp]->di->y;
	di->angle = ss->stack[ss->sp]->di->angle;
	di->distance = 0;
	//new a scope
	ss->sp++;
	ss->stack[ss->sp] = (struct scope *)malloc(sizeof(struct scope));
	ss->stack[ss->sp]->var_table = NULL;
	ss->stack[ss->sp]->di = di;

	for(int i = from; i <= to; i++)
	{
	  struct INSTRCTLST *next = ins->ins.doloop->il;

	  add_var(ss, ins->ins.doloop->var, i);

	  while(next != NULL)
	  {
	    draw_INSTRUCTION(screen, next->ins, ss);
	    next = next->next;
	  }
	}

	ss->sp--;
      }
      break;
  }
  return 0;
}
