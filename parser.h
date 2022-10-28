#ifndef _PARSER_H
#define _PARSER_H
# define MODENUM 5
char **filenames_parser(int* filenum, char **arguments, int argc, int index);
int mode_parser(int *modes, char **arguments, int argc);
int help_command();
#endif
