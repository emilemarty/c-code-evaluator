#ifndef _HELPERS_FILE_H
#define _HELPERS_FILE_H
char fgetc_plus(FILE *fp);
char fgetc_plus_fn(FILE *fp);
int fget_token(char *buffer, int size, FILE *fp);
char *get_this_line_code(FILE *fp, int line_num);
char fgetc_plus_comment(FILE *fp);
#endif
