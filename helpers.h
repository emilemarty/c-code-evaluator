#ifndef _HELPERS_H
#define _HELPERS_H
char **tokenize_with_size(char *str, int* size);
int at_line(FILE *f);
void to_upper(char* string);
int in_list(char *string, char **stringlist, int list_size);
#endif
