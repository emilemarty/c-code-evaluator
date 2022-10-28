#ifndef _HELPERS_TYPE_H
#define _HELPERS_TYPE_H
extern char **types;
extern int type_number;
char **get_defined_types(char *filename, int *num_element);
int collect_all_types(char **filenames, int filenumber);
#endif
