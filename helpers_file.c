#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include"main.h"
// Programmer : Yuyan Chen, Jiaxuan Chen
// Created : at 7/1/2021
// Purpose:
// provide helpers about files for other files
//
// Modifications:
// Initial Date Short Description 
// <none>

char fgetc_plus(FILE *fp){

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
    if (cur == '/' && next == '/') {
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

char fgetc_plus_fn(FILE *fp){
    // ---------------------------------------------------
    // Created : 5/21/2021
    // Purpose: 
    // skip the contents between ' ', " ", /**/ and after //
    // differnt from fgetc_plus, this function reads the contents between brackets
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
    if (cur == '/' && next == '/') {
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
    return cur;
}

char fgetc_plus_comment(FILE *fp){
    // ---------------------------------------------------
    // Created : 5/21/2021
    // Purpose: 
    // skip the contents between ' ', " ", /**/ and after //
    // differnt from fgetc_plus, this function reads the contents between brackets
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
    if (cur == '\n') return '\n';
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
            if (cur == '\"')  return fgetc_plus_comment(fp); 
        }
    }
    return cur;
}
int fget_token(char *buffer, int size, FILE *fp){
    // ---------------------------------------------------
    // Created : at 7/4/2021
    // Purpose:
    // get file characters as tokens
    //
    // Parameters : buffer offset and file pointer
    // Returns : 1 for good
    // Side-effects : NULL

    char cur;//the current char
    for(int i=0; i<size; i++){
        buffer[i]='\0';
    }
    if(feof(fp)) return EOF;
    do{
        cur = fgetc(fp);
    }while(cur==' ' || cur == '\n' || cur == '*');
    fseek(fp, -1, SEEK_CUR);
    for(int i=0; i<size; i++){
        cur = fgetc_plus_fn(fp);
        if(cur == '*') continue;
        if(cur==' ' || cur=='\n' || cur == EOF) break;
        buffer[i]=cur;
        
    }
    if(strcmp(buffer, "struct")==0){
        buffer[6]=' ';
        for(int i=7; i<size; i++){
            cur=fgetc_plus_fn(fp);
            if(cur == '*') continue;
            if(cur==' ' || cur=='\n' || cur == EOF) break;
            if(cur==EOF) return EOF;
            buffer[i]=cur;
        }
    }
    return 1;
}

char *get_this_line_code(FILE *fp, int line_num){
    // ---------------------------------------------------
    // Created : at 7/4/2021
    // Purpose:
    // get line_num th line of code
    //
    // Parameters : a file pointer the wanted line number
    // Returns : a line of code
    // Side-effects : NULL
    if(feof(fp)) return "FILE END\n";
    int ini = ftell(fp);//back up the current position of the file pointer
    rewind(fp);
    int size=1;//record the size of the line
    int line = 1;//count which line are we at
    char chr;//current char we are on
    char *result = malloc(1024*sizeof(char));
    while(line <= line_num){
        if(feof(fp)) return NULL;
        fgets(result, 1024, fp);
        line++;
    }
    fseek(fp, ini, SEEK_SET);
    return result;
}


