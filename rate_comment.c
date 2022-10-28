#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"output.h"
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include"main.h"
// Programmer :  Jiaxuan Chen
// Created :  at 6/03/2021
// Purpose: rate comments of the input files
//
//
// Modifications: 
// Initial Date Short Description 
// <none>

int one_comment_per_func(char *filename, int *result);
int count_globals_and_funcs(char *filename);
int count_comment_line(char *filename);

int rate_comment(char **filenames, int file_num){
    // ---------------------------------------------------
    // Created : 5/26/2021 main rating function for comment
    // Purpose:
    // rate all the comment of the input files
    // 
    // Parameters : all the filenames and the number of files 
    // Returns : constant value
    // Side-effects : no side effects


    /*after you finish evaluate the files
    rememeber to use function fill_comment_summary to record the summary of your evaluation
    and use flag_code to flag the improper code
    these two functions are in output.c, check them out! 
    and modify it if you want!*/
    printf("rate_comment called\n");
    int num=0;//the total score
    int hits=0;//the score get
    int result[2];//the result for rating comment
    int comment_line;//number of comment line
    int globals;//number of global and funcs
    for(int i=0; i<file_num; i++){
        result[0]=0;
        result[1]=0;
        num++;
        comment_line=count_comment_line(filenames[i]);
        globals=count_globals_and_funcs(filenames[i]);
        if(comment_line>globals){
            hits++;
        }else{
            flag_code(filenames[i], 0, "Not enough comment in this file", "Not Applicable");
        }
        one_comment_per_func(filenames[i], result);
        num+=result[1];
        hits+=result[0];
    }
    fill_comment_summary(hits, num);
    return 1;
}

int one_comment_per_func(char *filename, int *result){
    // ---------------------------------------------------
    // Created : at 7/2/2021
    // Purpose:
    // find out if a function has at least one line of comment
    // 
    // Parameters : input filename 
    // Returns : constant value
    // Side-effects : no side effects

    FILE *fp;//file pointer 
    struct FUNC *cur_func;//current function we are dealing with
    int record=0;//record the position fo the file pointer
    int total=0;//total score
    int grade=0;//grade get
    int index;//index to iterate through the function
    int line_num;//the line number the pointer is at
    char func_info[100];//to be put into flag_code
    char pre;//last char we are on
    char cur;//next char we are on
    fp=fopen(filename, "r");
    if(fp==NULL) error_dealer(-2);
    do{
        fseek(fp, record, SEEK_SET);
        cur_func=get_next_function(fp);
        record=ftell(fp);
        if(cur_func==NULL) break;
        total++;
        index=0;
        line_num=at_line(fp);
        sprintf(func_info, "Function: %s %s\n", cur_func->name, cur_func->parameters==NULL? "()":cur_func->parameters);
        fseek(fp, cur_func->func_start, SEEK_SET);
        for(index=0; index<cur_func->offset; index++){
            pre=cur;
            cur=fgetc(fp);
            if(cur=='"') {
                do{cur=getc(fp);} while(cur!='"');
                cur=fgetc(fp);
                continue;
                //ignore string
            }
            if(pre=='/' && (cur=='/' || cur=='*')) {
                grade++;
                break;
            }
        }
        if(index==cur_func->offset){
            flag_code(filename, line_num, "This function has no comment, 1 hit missed", func_info);
        }
    }while(cur_func!=NULL);
    result[0]+=grade;
    result[1]+=total;
    return 1;
}

int count_comment_line(char *filename){
    // ---------------------------------------------------
    // Created : at 7/4/2021
    // Purpose:
    // count the total comment line number of the input file
    // 
    // Parameters : input filename 
    // Returns : number of lines
    // Side-effects : no side effects

    int count = 0;//count the comment
    char buffer[1024];//to store the line
    char buffer1[1024];//store the processed buffer
    char cur;//current char we are on
    char pre;//last char we are on
    FILE *fp;//file pointer
    fp = fopen(filename, "r");
    if(fp == NULL) {
        error_dealer(-2);
    }
    do{
        pre = cur;
        cur = fgetc_plus_comment(fp);
        if(feof(fp)) break;
        if(pre=='/' && cur=='/'){
            while(cur!='\n' && cur!=EOF){
                cur=fgetc_plus_comment(fp);
            }
            count++;
        }else if(pre=='/' && cur=='*'){
            while(pre!='*' && cur!='/' && cur!=EOF){
                pre = cur;
                cur = fgetc_plus_comment(fp);
                if(cur=='\n') count++;
                if(feof(fp)) break;
            }
            count++;
        }
    }while(!feof(fp));
    fclose(fp);
    return count;
}

int count_globals_and_funcs(char *filename){
    // ---------------------------------------------------
    // Created : at 7/4/2021
    // Purpose:
    // count the number of globals and functions of the input file 
    // 
    // Parameters : input filename 
    // Returns : number wanted 
    // Side-effects : no side effects

    FILE *fp1, *fp2;//file pointers of the file, 1 for func, 2 for var
    struct FUNC *cur_func=NULL;//the current function we are processing
    struct VAR_LIST *var=NULL;//the var list of globals
    int count=0;//count the number
    fp1=fopen(filename, "r");
    fp2=fopen(filename, "r");
    if(fp1==NULL) error_dealer(-2);
    do{
        cur_func=get_next_function(fp1);
        if(cur_func==NULL) break;
        count++;
        while(ftell(fp2)<cur_func->func_start){
            var=get_var(fp2);
            if(ftell(fp2)>=cur_func->func_start) break;
            if(var==NULL) {
                break;
            } else{
                while(var!=NULL){
                    count++;
                    var=var->next;
                }
            }
        }
        fseek(fp2, cur_func->func_start+cur_func->offset, SEEK_SET);
    }while(!feof(fp1));
    fclose(fp1);
    fclose(fp2);
    return count;
}
