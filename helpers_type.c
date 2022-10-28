#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include"output.h"
#include"main.h"
// Programmer : Yuyan Chen, Jiaxuan Chen
// Created : at 7/1/2021
// Purpose:
// provide helpers about types for other files
//
// Modifications:
// Initial Date Short Description 
// <none>

char **types;
int type_number=0;

char fgetc_type(FILE *fp){

    // ---------------------------------------------------
    // Created : on 5/21/2021
    // Purpose: 
    // skip the contents between ' ', " ", { }, /**/ and after //
    //
    // Parameters : file pointer
    // Returns : same as fgetc after skipping the contents
    // Side-effects : move the file pointer by one
    //                note that this function will read \n as space
    int brackets = 0; // +1 if the current character is {, -1 if the current character is }
    int cur; // to store the current character
    int next; // to store the next character

    cur = fgetc(fp);
    if (cur == EOF) return EOF;
    next = fgetc(fp);
    if (feof(fp)) return cur;
    fseek(fp, -1, SEEK_CUR);
    if (cur == '\n') return ' ';

    // skip character
    if (cur == '\'') {
        if(next=='\\'){
            for(int i=0; i<4; i++) cur=fgetc(fp);
            return cur;
        }
        else{
            for(int i=0; i<3; i++) cur=fgetc(fp);
            return cur;
        }
        
    }

    // skip string
    if (cur == '\"') {
        while(1){
            cur = fgetc(fp);
            if (cur == '\"')  return fgetc_plus(fp); 
        }
    }

    // skip single-line comment
    if ((cur == '/' && next == '/') || cur=='#') {
        fseek(fp, 1, SEEK_CUR);
        while(1){
            cur = fgetc(fp);
                if (cur == '\n') return ' ';
                if (cur == EOF) return cur;
        }
    }

    // skip multi-line comment
    if (cur == '/' && next == '*') {
        fseek(fp, 1, SEEK_CUR);
        while(1){
            cur = fgetc(fp);
            next = fgetc(fp);
            if (feof(fp)) return cur;
            fseek(fp, -1, SEEK_CUR);
            if (cur == '*' && next == '/') {
                fseek(fp, 1, SEEK_CUR);
                return fgetc_plus(fp);
            }
        }
    }
    
    // skip bracket
    if (cur == '{'){
        int temp; // to temporarily store a character
        brackets += 1;
        while(1){
            temp = fgetc_plus(fp);
            if (temp == '{') brackets ++;
            if (temp == '}') brackets --;
            if (brackets == 0){
                fseek(fp, -1, SEEK_CUR);
                return cur;
            }
        }
    }
    return cur;
}


char **get_defined_types(char *filename, int *num_element){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // get all types from the input c code
    //
    // Parameters : filename to read, num of element that will be stored
    // Returns : a string array of types
    // Side-effects : NULL
    *num_element=0;
    FILE *fp;
    int cur;
    int buffer_len = 1024;
    char buffer[buffer_len];
    char** defined_types = malloc(sizeof(char*));
    char *ptr1, *ptr2, *ptr3, **words;
    int size = 0, count = 0;

    fp = fopen(filename, "r");

    if (fp == NULL){
        return NULL;
    }

    while (1){
        cur = fgetc_type(fp);
        if (cur == EOF) break;
        if (cur != ';'){
            //count += 1;
            if(cur=='{') {
                buffer[count++]=' ';
                buffer[count++]='{';
            } else if(cur=='}'){
                buffer[count++]='}';
                buffer[count++]=' ';
            } else{
                buffer[count++] = cur;
            }
        }
        else{
            //count += 1;
            buffer[count++] = 0;
            count = 0;
            ptr1 = strstr(buffer, "struct");
            ptr2 = strstr(buffer, "typedef");
            ptr3 = strstr(buffer, "{}");

            if (ptr1 != NULL || ptr2 != NULL) {        
                words = tokenize_with_size(buffer, &size);    
                /*
                case 1: struct a{...}; 
                        store "struct a";
                */
                if (ptr1 != NULL && ptr3 != NULL && ptr2 == NULL){
                    *num_element += 1;
                    defined_types = realloc(defined_types, *num_element * sizeof(char*));
                    char *result;
                    char *temp = strdup(words[1]);
                    result = malloc(strlen("struct ") + strlen(temp) + 1);
                    sprintf(result, "struct %s", temp);
                    defined_types[*num_element -1] = strdup(result);
                    free(temp);
                }

                /*
                case 2: typedef a b
                        store "b"
                */
                else if (ptr2 != NULL && ptr3 == NULL){
                    *num_element += 1;
                    defined_types = realloc(defined_types, *num_element * sizeof(char*));
                    defined_types[*num_element -1] = strdup(words[size-1]);
                    for(int j=0; j<strlen(words[size-1]); j++){
                        if(words[size-1][j]>='a' && words[size-1][j]<='z'){
                            flag_code(filename, at_line(fp), "User defined type names must be all CAPS", words[size-1]);
                            break;
                        }
                    }
                }

                /*
                case 3: typedef struct {} a;
                        store "a"
                */

                else if (ptr1 != NULL && ptr2 != NULL && ptr3 != NULL){

                    if (words[2][0] == '{'){
                        *num_element += 1;
                        defined_types = realloc(defined_types, *num_element * sizeof(char*));
                        defined_types[*num_element -1] = strdup(words[size-1]);
                    }

                    else{
                    /*
                    case 4: typedef struct a {} b;
                            store "struct a" and "b"
                    */
                    char *result, *dup;
                    *num_element += 1;
                    defined_types = realloc(defined_types, *num_element * sizeof(char *));
                    dup = strdup(words[2]);
                    result = malloc(strlen("struct ") + strlen(dup) + 1);
                    sprintf(result, "struct %s", dup);
                    defined_types[*num_element-1] = strdup(result);
                    free(dup);
                    *num_element += 1;
                    defined_types = realloc(defined_types, *num_element * sizeof(char*));
                    defined_types[*num_element -1] = strdup(words[size-1]);
                    }  
                }
                size = 0;
            }
        }  
    }
    fclose(fp);
    return defined_types;
};

int collect_all_types(char **filenames, int filenumber){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // get all default and user defined types, and store it in a global variable
    //
    // Parameters : filename list, list size
    // Returns : if succeed
    // Side-effects : NULL
    char *default_types[8]={"int", "char", "void", "float", "double", "short", "long", "FILE"};
    //the default types
    type_number=8;//record the total type number
    int index=0;//the index of the type list position
    char **buffer;//store the return value of get_defined_types
    int num_element;//the number of element of the return value of get_defined_type
    types=malloc(type_number*sizeof(char *));
    for(int i=0; i<8; i++){
        types[i]=strdup(default_types[i]);
        index++;
    }
    for(int i=0; i<filenumber; i++){
        buffer = get_defined_types(filenames[i], &num_element);
       
        for(int j=0; j<num_element; j++){
            if(in_list(buffer[j], types, type_number)==0){
                type_number++;
                types=realloc(types, type_number*sizeof(char *));
                types[index++]=strdup(buffer[j]);
            }
        }
        
    }
    free(buffer);
    return 1;
}

