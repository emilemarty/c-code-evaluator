#ifndef _HELPERS_VAR_H
#define _HELPERS_VAR_H
struct VAR{
    char *type;
    char *name;
};
// record the variable
struct VAR_LIST{
    struct VAR *cur;
    struct VAR_LIST *next;
};
// a linked list for struct VAR
struct FUNC{
    char *name;
    char *parameters;
    int func_start;
    int offset;
};
//record the info of functions
struct FUNC_LIST{
    char *name;
    struct FUNC_LIST *next;
};
// a linked list for struct FUNC

struct FUNC *get_next_function(FILE *fp);
struct FUNC *get_next_declaration(FILE *fp);
int append_funclist(struct FUNC_LIST *head, struct FUNC *new);
int contains_funclist(struct FUNC_LIST *head, struct FUNC *key);
int contains_varlist(struct VAR_LIST *head, struct VAR *key);
struct VAR_LIST *get_var(FILE *fp);
#endif
