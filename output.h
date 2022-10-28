#ifndef _OUTPUT_H
#define _OUTPUT_H
int create_output_files();
int fill_modular_summary(int hits, int nums);
int fill_indentation_summary(int hits, int nums);
int fill_comment_summary(int hits, int nums);
int fill_doc_summary(int hits, int nums);
int fill_var_summary(int hits, int nums);
int fill_testcases_summary(int hits, int nums);
int flag_code(char *filename, int line_number, char* error, char *code);
int give_output();
#endif
