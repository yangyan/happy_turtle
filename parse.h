#ifndef PARSE
#define PARSE

#define VARMAXLEN 256

struct VARNUM
{
  enum VARNUM_TYPE {VN_VAR, VN_NUM} type;
  union {
    double num;
    char var;
  } vn;
};

struct DO
{
  char var;
  struct VARNUM *from;
  struct VARNUM *to;
  struct INSTRCTLST *il;
};

struct POLISH 
{
  struct POLISH *next;
  enum POLISH_TYPE {PO_VN, PO_OP} type;
  union {
    struct VARNUM *vn;
    char op;
  }po;
};

struct SET
{
  char var;
  struct POLISH *po;
};

struct FD
{
  struct VARNUM *num;
};

struct LT
{
  struct VARNUM *num;
};

struct RT
{
  struct VARNUM *num;
};

struct INSTRUCTION
{
  enum INSTRCTION_TYPE {IT_FD, IT_LT, IT_RT, IT_DO, IT_SET} type;
  union {
    struct FD *fd;
    struct LT *lt;
    struct RT *rt;
    struct DO *doloop;
    struct SET *set;
  } ins;
};

struct INSTRCTLST
{
  struct INSTRCTLST *next;
  struct INSTRUCTION *ins;
};

struct MAIN
{
  struct INSTRCTLST *next;
};

extern struct MAIN *read_MAIN(void);

#endif
