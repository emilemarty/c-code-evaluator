#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"output.h"
#include"main.h"
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
// Programmer : Jiaxuan Chen
// Created : 5/15/2021 to rate documentation
// Purpose:
// rate the documentation of the input file
//
// Modifications:
// Initial Date Short Description 
// <none>

char **tokenize_doc(char *string);
int is_in_list(char *string, char **list, int length);
int rate_doc_head(char *filename);
int *rate_doc_func(char *filename);
char fgetc_doc(FILE *fp);

int rate_doc(char **filenames, int file_num){
    // ---------------------------------------------------
    // Created : 5/26/2021 main rating function for documentation
    // Purpose:
    // rate all the documentation of the input files
    // 
    // Parameters : all the filenames and the number of files 
    // Returns : constant value
    // Side-effects : no side effects

    printf("rate_doc called\n");
    int num=0;//the total score
    int hits=0;//the hits 
    int *result=NULL;//the result to be filled by rating functions
    for(int i=0; i<file_num; i++){
        num+=4;
        hits+=rate_doc_head(filenames[i]);
        result=rate_doc_func(filenames[i]);
        num+=result[1];
        hits+=result[0];
    }
    fill_doc_summary(hits, num);
    if(result!=NULL) free(result);
    return 1;
}

char **tokenize_doc(char *string) {
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // tokenize a string with ' ', '\n', ':'  as delimiter
    // 
    // Parameters : a string
    // Returns : tokens
    // Side-effects : no side effects

    char **tokens;//to be returned
    char buffer[1000];//buffer to store the line
    int index1=0, index2=0, index3=0;//iterators
    while(string[index1]!=0){
        while(string[index1]==' ' || string[index1]=='\n' || string[index1]==':') index1++;
        if(string[index1]==0) break;
        while(string[index1]!=' ' && string[index1]!=0 && string[index1]!='\n') index1++;
        index3++;
    }
    tokens=malloc(sizeof(char *)*(index3));
    index1=0, index3=0;
    while(string[index1]!=0){
        while(string[index1]==' ' || string[index1]=='\n' || string[index1]==':') index1++;
        if(string[index1]==0) break;
        index2=0;
        while(string[index1]!=' ' && string[index1]!=0 && string[index1]!='\n') {
            buffer[index2]=string[index1];
            index2++;
            index1++;
        }
        buffer[index2]=0;
        tokens[index3]=strdup(buffer);
        index3++;
    }
    tokens[index3]=NULL;
    return tokens;
}

int is_in_list(char *string, char **list, int length){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // find out if string is a substring of a string in the string list
    //
    // Parameters : string, string array, array size
    // Returns: true or false
    // Side-effects : NULL

    char *sub;//for strstr
    for(int i=0; i<length; i++){
        sub=strstr(string, list[i]);
        if(sub!=NULL) return 1;
    }
    return 0;
}

int rate_doc_head(char *filename){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // rate the head documentation of a file
    //
    // Parameters : filename to rate
    // Returns: if succeed
    // Side-effects : NULL

    int grade=0;//the grade get
    int index;//index for comment
    int size;//size of the comment
    int counter;//count the token for each section
    char cur;//current char
    char pre;//last char
    char *sub;//for strstr
    char *list[4] = {"Programmer", "Created", "Purpose", "Modifications"};
    //the tokens
    char **tokens=NULL;//store the tokenized tokens
    FILE *fp = fopen(filename, "r");//the file pointer
    do {cur=fgetc_doc(fp); } while(cur==' ' || cur=='\t' || cur =='\n');
    size=ftell(fp);
    if(size==0){
        flag_code(filename, 0, "No documentation for this file, 4 hits missed", "-FILE START-\n");
        return 0;
    }
    char comment[size];
    fseek(fp, 0, SEEK_SET);
    for(int i=0; i<size; i++) comment[i]=fgetc(fp);
    for(index=1; index<size; index++){
        cur=comment[index];
        pre=comment[index-1];
        if(cur=='/' && pre=='/') break;
        if(cur=='*' && pre=='/') break;
    }
    if(index==size){
        flag_code(filename, 0, "No documentation for this file", "-FILE START-\n");
        return 0;
    }
    sub = strstr(comment, "Programmer");
    if(sub!=NULL){
        tokens=tokenize_doc(sub);
        counter=0;
        index=1;
        while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
            index++;
            if(strcmp(tokens[index], "//")==0) continue;
            counter++;
        }
        if(counter<2){
            flag_code(filename, at_line(fp), "Programmer tokens not sufficient, 1 hit missed", "--FILE START--\n");
        }else grade++;
    }
    else flag_code(filename, at_line(fp), "Programmer section missing, 1 hit missed", "--FILE START--\n");

    sub = strstr(comment, "Created");
    if(sub!=NULL){
        tokens=tokenize_doc(sub);
        counter=0;
        index=1;
        while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
            index++;
            if(strcmp(tokens[index], "//")==0) continue;
            counter++;
        }
        if(counter<2){
            flag_code(filename, at_line(fp), "Created tokens not sufficient, 1 hit missed", "--FILE START--\n");
        }else grade++;
    }
    else flag_code(filename, at_line(fp), "Created section missing, 1 hit missed", "--FILE START--\n");
    
    sub = strstr(comment, "Purpose");
    if(sub!=NULL){
        tokens=tokenize_doc(sub);
        counter=0;
        index=1;
        while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
            index++;
            if(tokens[index]==NULL) break;
            if(strcmp(tokens[index], "//")==0) continue;
            counter++;
        }
        if(counter<4){
            flag_code(filename, at_line(fp), "Purpose tokens not sufficient, 1 hit missed", "--FILE START--\n");
        }else grade++;
    }
    else flag_code(filename, at_line(fp), "Purpose section missing, 1 hit missed", "--FILE START--\n");

    sub = strstr(comment, "Modifications");
    if(sub!=NULL){
        tokens=tokenize_doc(sub);
        counter=0;
        index=1;
        while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
            index++;
            if(tokens[index]==NULL) break;
            if(strcmp(tokens[index], "//")==0) continue;
            counter++;
        }
        if(counter<3){
            flag_code(filename, at_line(fp), "Modifications tokens not sufficient, 1 hit missed", "--FILE START--\n");
        }else grade++;
    }
    else flag_code(filename, at_line(fp), "Modifications section missing, 1 hit missed", "--FILE START--\n");
    if(tokens!=NULL) free(tokens);
    return grade;
}

int *rate_doc_func(char *filename){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // rate function documentation 
    //
    // Parameters : filename to rate
    // Returns: if succeed
    // Side-effects : NULL

    FILE *fp;//file pointer
    struct FUNC *cur_func;//current function we are processing
    char *comment=NULL;//the comment
    char *sub;//for strstr
    char **tokens=NULL;//tokens of the comment section
    char cur;//current char
    char pre;//last char
    char func_info[100];//for flag code
    char *list[6] = {"Programmer", "Created", "Purpose", "Parameters", "Returns", "Side-effects"};
    //comment sections
    int size;//size of comment
    int line_num;//current line of file we are on
    int index;//index for comment
    int counter;//count the token number
    int total=0;//total score
    int grade=0;//score get
    int *result=malloc(2*sizeof(int));//to be returned
    int record=0;//record the position of the file pointer
    fp = fopen(filename, "r");
    if(fp==NULL) error_dealer(-2);
    do{
        size=0;
        line_num=0;
        index=0;
        fseek(fp, record, SEEK_SET);
        cur_func=get_next_function(fp);
        if(cur_func==NULL) break;
        record=cur_func->func_start+cur_func->offset+1;
        total+=5;
        sprintf(func_info, "Function: %s %s\n", cur_func->name, cur_func->parameters==NULL? "()":cur_func->parameters);
        
        fseek(fp, cur_func->func_start, SEEK_SET);
        line_num=at_line(fp);
        do {cur=fgetc_plus(fp); } while(cur==' ' || cur=='\t' || cur =='\n');
        size=ftell(fp)-cur_func->func_start;
        if(size==0){
            flag_code(filename, line_num, "No documentation for this function, 5 hits missed", func_info);
            continue;
        }
        if(comment==NULL) {
            comment=malloc(size*sizeof(char));
        }
        else {
            free(comment);
            comment=NULL;
            comment=malloc(size*sizeof(char));
        }
        fseek(fp, cur_func->func_start, SEEK_SET);
        for(index=0; index<size; index++) comment[index]=fgetc(fp);
        for(index=1; index<size; index++){
            cur=comment[index];
            pre=comment[index-1];
            if(cur=='/' && pre=='/') break;
            if(cur=='*' && pre=='/') break;
        }
        if(index==size){
            flag_code(filename, line_num, "No documentation for this function, 5 hits missed", func_info);
            continue;
        }
     
        sub=strstr(comment, "--------------------");
        total++;
        if(sub==NULL) flag_code(filename, line_num, "No horizontal line or horizontal line not long enough, 1 hit missed", func_info);
        else grade++;
        sub=strstr(comment, "Programmer");
        if(sub!=NULL){
            total++;
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<2){
                flag_code(filename, line_num, "Programmer tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }

        sub = strstr(comment, "Created");
        if(sub!=NULL){
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<2){
                flag_code(filename, line_num, "This function's created tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }
        else flag_code(filename, line_num, "Created section missing, 1 hit missed", func_info);

        sub = strstr(comment, "Purpose");
        if(sub!=NULL){
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(tokens[index]==NULL) break;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<4){
                flag_code(filename, line_num, "Purpose tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }
        else flag_code(filename, line_num, "Purpose section missing, 1 hit missed", func_info);

        sub = strstr(comment, "Parameters");
        if(sub!=NULL){
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(tokens[index]==NULL) break;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<2){
                flag_code(filename, line_num, "Parameters tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }
        else flag_code(filename, line_num, "Parameters section missing, 1 hit missed", func_info);

        sub = strstr(comment, "Returns");
        if(sub!=NULL){
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(tokens[index]==NULL) break;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<2){
                flag_code(filename, line_num, "Returns tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }
        else flag_code(filename, line_num, "Returns section missing, 1 hit missed", func_info);

        sub = strstr(comment, "Side-effects");
        if(sub!=NULL){
            tokens=tokenize_doc(sub);
            counter=0;
            index=1;
            while(tokens[index]!=NULL && is_in_list(tokens[index], list, 4)!=1){
                index++;
                if(tokens[index]==NULL) break;
                if(strcmp(tokens[index], "//")==0) continue;
                counter++;
            }
            if(counter<1){
                flag_code(filename, line_num, "Side-effects tokens not sufficient, 1 hit missed", func_info);
            }else grade++;
        }
        else flag_code(filename, line_num, "Side-effects section missing, 1 hit missed", func_info);
    }while(cur_func!=NULL);
    if(tokens!=NULL) free(tokens);
    if(comment!=NULL) free(comment);
    result[0]=grade;
    result[1]=total;
    return result;
}

char fgetc_doc(FILE *fp){

    // ---------------------------------------------------
    // Created : at 6/23/2021
    // Purpose: 
    // skip the contents between ' ', " ", { }, /**/ and after //
    //
    // Parameters : file pointer
    // Returns : same as fgetc after skipping the contents
    // Side-effects : move the file pointer by one
    //                note that this function will read \n as space
    int brackets = 0;//counter for {}
    int cur, next;//current and last char

    cur = fgetc(fp);
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

    if (cur == '#') {
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
        int temp;
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
