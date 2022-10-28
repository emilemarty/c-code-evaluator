#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include"main.h"
// Programmer : all team members
// Created : at 5/15/2021 
// Purpose:
// provide helpers for other files
//
// Modifications:
// Initial Date Short Description 
// <none>

char **tokenize_with_size(char *str, int* size) {
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // tokenize a string 
    // delimiters: space only
    // Parameters : string and a int pointer to store the length of the returned array
    // Returns : a string array of tokens
    // Side-effects : none

    char **tokens; // to store all tokens
    char *token; // to store a single token
    const char S[2] = " "; // a space 
    int index = 0; // the index of each token
    char *dup1 = strdup(str); // a duplicate of the input string 
    char *dup2 = strdup(str); // a duplicate of the input string 

    token = strtok(dup1, S);
    while( token != NULL ) {
        *size+=1;
        token = strtok(NULL, S);
    }

    tokens = malloc(*size * sizeof(char*));
    token = strtok(dup2, S);
    while( token != NULL ) {
        tokens[index++] = token;
        token = strtok(NULL, S);
    }

    return tokens;
}

void to_upper(char* string){
    // ---------------------------------------------------
    // Created : on 5/21/2021
    // Purpose: 
    // convert letter to upper case
    //
    // Parameters : string
    // Returns : void func
    // Side-effects : none
    const char OFFSET = 'a' - 'A'; // offset between a lowercase letter and the uppercase letter
    while (*string){
        *string = (*string >= 'a' && *string <= 'z') ? *string -= OFFSET : *string;
        string++;
    }
}

int at_line(FILE *fp){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // find out which line the file descriptor is on
    //
    // Parameters : a file pointer
    // Returns : line number
    // Side-effects : NULL

    int ini;
    int cur = 0;//cur position of the file pointer
    int line = 1;//count the line number
    char chr;//current char we are on
    if(feof(fp)) {
        rewind(fp);
        while(!feof(fp)){
            chr=fgetc(fp);
            if(chr=='\n') line++;
        }
        return line;
    }
    else{
        ini = ftell(fp);//record the position of fp
        rewind(fp);
        while(cur < ini){
            chr=fgetc(fp);
            if(chr=='\n') line++;
            cur = ftell(fp);
        }
        fseek(fp, ini, SEEK_SET);
        return line;
    }
}

int in_list(char *string, char **stringlist, int list_size){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // find out if string is an element in the string list
    //
    // Parameters : string, string array, array size
    // Returns : true or false
    // Side-effects : NULL
    if(string[strlen(string)-1]!='*'){
        for(int i=0; i<list_size; i++){
            if(strcmp(string, stringlist[i])==0) return 1;
        } 
        return 0;
    }
    int counter = strlen(string);//record the length of the string
    for(int i=strlen(string)-1; i>=0; i--){
        if(string[i]=='*') counter--;
    }
    char new_string[counter]; // to store the new string 
    for(int i=0;i<counter; i++){
        new_string[i]=string[i];
    }
    for(int i=0; i<list_size; i++){
        if(strcmp(new_string, stringlist[i])==0) return 1;
    } 
    return 0;
}

