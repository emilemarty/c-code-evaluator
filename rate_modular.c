#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rate_modular.h"
#include "helpers_file.h"
#include "helpers_type.h"
#include "helpers_var.h"
#include "helpers.h"
#include "output.h"

// Programmer : Emile Marty
// Created : at 2021-05-17
// Purpose:
// Rates the program on Modular Programming rules
//
// Modifications:
// Initial Date Short Description
// <none>



int rate_modular(char **cfiles, char **hfiles, int file_num)
{
    // ---------------------------------------------------
    // Created : Emile Marty
    // Purpose: Main rating function
    //
    //
    // Parameters : list of filenames to rate, size of the list
    // Returns : 0 -> failure, 1 -> success
    // Side-effects : Sends the modular programming summary to output

    printf("rate_modular called\n");
    int score = 0;
    struct FUNC_LIST *head = malloc(sizeof(struct FUNC_LIST *));
    struct FUNC *cur = NULL;
    struct VAR_LIST *public_var = NULL;
    struct VAR_LIST *source_var = NULL;
    for (int i = 0; i < file_num; i++)
    {
        char *source = cfiles[i];
        char *header = hfiles[i];
        head->name = NULL;
        head->next = NULL;
        // Rule 1 - Matching header file
        FILE *fp = fopen(header, "r");
        if (fp == NULL)
        {
            flag_code(header, 0, "Every .c file must have a matching .h file", "Rule 1");
            continue;
        }

        // Rule 2 - Protect header from multiple includes
        int check = 0;
        char buffer1[100];
        if (fgets(buffer1, 100, fp) != NULL)
        {
            if (strstr(buffer1, "#ifndef") != NULL)
            {
                check++;
            }
        }
        if (fgets(buffer1, 100, fp) != NULL)
        {
            if (strstr(buffer1, "#define") != NULL)
            {
                check++;
            }
        }
        if (check != 2)
        {
            flag_code(header, at_line(fp), "Every matching .h file must be protected from multiple includes", "Rule 2");
            continue;
        }
        fclose(fp);
       
        // Rule 3 - No duplicate function protypes
        fp = fopen(header, "r");
        cur = get_next_declaration(fp);
        check = 1;
        while (cur != NULL)
        {
            if (append_funclist(head, cur) == 0)
            {
                check = 0;
                break;
            }
            cur = get_next_declaration(fp);
            
        }
        if (check == 0)
        {
            flag_code(header, at_line(fp), "A .c file cannot have function prototypes from another .c file", "Rule 3");
            continue;
        }
        fclose(fp);;
        // Rule 4 - No extern statements
        fp = fopen(source, "r");
        char buffer2[1024];
        int start = at_line(fp);
        check = 1;
        while (!feof(fp))
        {
            fget_token(buffer2, 1024, fp);
            if (strcmp(buffer2, "extern") == 0)
            {
                check = 0;
                break;
            }
        }
        if (check == 0)
        {
            flag_code(source, at_line(fp), "A .c file cannot have an extern statement", "Rule 4");
            continue;
        }
        fclose(fp);
        
        // Rule 5 - At least one private function
        fp = fopen(source, "r");
        cur = get_next_function(fp);
        check = 0;
        while (cur != NULL)
        {
            if (contains_funclist(head, cur) == 0)
            {
                check = 1;
                break;
            }
            else
                cur = get_next_function(fp);
        }
        if (check == 0)
        {
            flag_code(source, 0, "There must be at least one private function in a .c file", "Rule 5");
            continue;
        }
        fclose(fp);
        // Rule 6 - At least one private global variable
        if(public_var!=NULL) free(public_var);
        if(source_var!=NULL) free(source_var);
        FILE *hd = fopen(header, "r");
        public_var = get_var(hd);
        fclose(hd);
        FILE *sc = fopen(source, "r");
        source_var = get_var(sc);
        fclose(sc);
        check = 0;
        if (source_var != NULL)
        {
            while (source_var->next != NULL)
            {
                if (contains_varlist(public_var, source_var->cur) == 0)
                {
                    check = 1;
                    break;
                }
                source_var = source_var->next;
            }
            if (contains_varlist(public_var, source_var->cur) == 0)
                check = 1;
        }

        if (check == 0)
        {
            flag_code(source, 0, "There must be at least one private global variable in a .c file", "Rule 6");
            continue;
        }
        // Rule 7 - All includes at the beginning
        fp = fopen(source, "r");
        char buffer3[1000];
        check = 1;
        while (fgets(buffer3, 1000, fp) != NULL)
        {
            if (strstr(buffer3, "#define") == NULL)
                break;
        }
        while (fgets(buffer3, 1000, fp) != NULL)
        {
            if (strstr(buffer3, "#define") != NULL)
            {
                check = 0;
                break;
            }
        }
        if (check == 0)
        {
            flag_code(source, at_line(fp), "All #include directives must be at the beginning of the file", "Rule 7");
            continue;
        }
        fclose(fp);
        score++;
    }
    free(head);
    if(cur!=NULL) free(cur);
    if(public_var!=NULL) free(public_var);
    if(source_var!=NULL) free(source_var);
    fill_modular_summary(score, file_num);
    return 1;
}

