#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"output.h"
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
// Programmer :  Jiaxuan Chen
// Created :  at 7/3/2021
// Purpose:
// rate the variable naming of the input file
//
// Modifications:
// Initial Date Short Description 
// <none>

int *rate_user_defined_types();
int is_this_line_comment(FILE *fp, int line_num);
int does_this_line_have_comment(FILE *fp, int line_num);
int rate_defined_identifiers(FILE *fp, char *filename, int *rate_result);
int rate_const_naming(FILE *fp, char *filename, int *result);

int rate_var(char **filenames, char **all_files, int file_num){
    // ---------------------------------------------------
    // Created : at 7/3/2021
    // Purpose:
    // rate the naming of vaiables
    //
    // Parameters : c filenames, all h, c files, and the total file numbers
    // Returns : 1 for good
    // Side-effects : no side effects


    /*after you finish evaluate the files
    rememeber to use function fill_var_summary to record the summary of your evaluation
    and use flag_code to flag the improper code
    these two functions are in output.c, check them out! 
    and modify it if you want!*/
    printf("rate_var called\n");
    struct VAR_LIST *cur = NULL;//the var list to be filled with get_var
    FILE *fp;//file pointer
    int *result = rate_user_defined_types();//result of the rating function
    int sum=0;//total score
    int hits=0;//score get
    int line_num;//line number of the fp
    char *buffer=malloc(100*sizeof(char));//for flag code use
    for(int i=0; i<file_num; i++){
        fp = fopen(filenames[i], "r");
        while(!feof(fp)){
            cur = get_var(fp);
            if(feof(fp) && cur==NULL) break;
            line_num = at_line(fp);
            if(cur!=NULL && !feof(fp)){
                sum++;
                if(does_this_line_have_comment(fp, line_num) || is_this_line_comment(fp, line_num+1)){
                    hits++;
                }else {
                    flag_code(filenames[i], line_num, "This statement with declaration does not have comment", get_this_line_code(fp, line_num));
                }
            }
            while(cur!=NULL){ 
                sprintf(buffer, "VAR at line: %d: [type: %s, name: %s]\n", at_line(fp), cur->cur->type, cur->cur->name);
                sum++;
                if(strlen(cur->cur->name)>=2){
                    hits++;
                } else{
                    flag_code(filenames[i], at_line(fp), "Variable name too short", buffer);
                }
                cur = cur->next;
            }
        }
        fclose(fp);
    }
    for(int i=0; i<file_num*2; i++){
        fp = fopen(all_files[i], "r");
        if(fp==NULL) continue;
        rate_defined_identifiers(fp, all_files[i], result);
        rewind(fp);
        rate_const_naming(fp, all_files[i], result);
        fclose(fp);
    }
    sum+=result[1];
    hits+=result[0];
    free(result);
    free(buffer);
    fill_var_summary(hits, sum);
    return 1;
}

int *rate_user_defined_types(){
    // ---------------------------------------------------
    // Created : at 7/5/2021
    // Purpose:
    // rate the naming of user defined types
    //
    // Parameters : no inputs
    // Returns : a two-length char result[0] for hit ,result[1] for sum
    // Side-effects : no side effects, file pointer's position remains the same
    
    int *result=malloc(2*sizeof(int));//to be returned
    char *tmp;//for strstr
    result[0]=type_number-8;//the number of user defined types
    result[1]=type_number-8;//same as above
    if(type_number==8) return result;
    for(int i=8; i<type_number; i++){
        tmp = strstr(types[i], "struct");
        if(tmp!=NULL) {
            tmp = strchr(types[i], ' ');
        }
        else {
            tmp = types[i];
        }
        for(int j=0; j<strlen(tmp); j++){
            if(tmp[j]>='a' && tmp[j]<='z'){
                result[0]--;
                break;
            }
        }
    }
    return result;
}

int does_this_line_have_comment(FILE *fp, int line_num){
    // ---------------------------------------------------
    // Created : at 7/5/2021
    // Purpose:
    // does the line_num th line have comment?
    //
    // Parameters : file pointer and line number
    // Returns : 1 for true, 0 for false
    // Side-effects : no side effects, file pointer's position remains the same
    
    char *line = get_this_line_code(fp, line_num);//the line wanted
    if(line==NULL) return 0;
    if(strstr(line, "//")==NULL && strstr(line, "/*")==NULL){
        free(line);
        return 0;
    }else {
        free(line);
        return 1;
    }
}

int is_this_line_comment(FILE *fp, int line_num){
    // ---------------------------------------------------
    // Created : at 7/5/2021
    // Purpose:
    // find out if the line_num th line is all comment
    //
    // Parameters : file pointer, the wanted line number
    // Returns : 1 for true, 0 for false
    // Side-effects : no side effects

    if(does_this_line_have_comment(fp, line_num)==0) return 0;
    char *line = get_this_line_code(fp, line_num);//the line wanted
    if(line==NULL) return 0;
    for(int i=0; i<strlen(line)-1; i++){
        if(line[i]!=' ' && line[i]!='\t'){
            if(line[i]=='/' && line[i+1]=='/'){
                free(line);
                return 1;
            }else if(line[i]=='/' && line[i+1]=='*'){
                free(line);
                return 1;
            }else{
                free(line);
                return 0;
            }
        }
    }
    return 0;
}

char fgetc_plus_fn_for_var(FILE *fp){
    // ---------------------------------------------------
    // Created : at 7/5/2021
    // Purpose: 
    // skip the contents between ' ', " ", /**/ and after //
    // differnt from fgetc_plus, this function reads the contents between brackets
    // '\n' is not replaced as ' '
    // Parameters : file pointer
    // Returns : same as fgetc after skipping the contents
    // Side-effects : move the file pointer by one
    //                note that this function will read \n as space
    int brackets = 0;//number of {} 
    int cur, next;//current and last char

    cur = fgetc(fp);
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
            if (cur == '\"')  return fgetc_plus(fp); 
        }
    }

    // skip single-line comment
    if (cur == '/' && next == '/') {
        fseek(fp, 1, SEEK_CUR);
        while(1){
            cur = fgetc(fp);
                if (cur == '\n') return '\n';
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

int rate_defined_identifiers(FILE *fp, char *filename, int *rate_result){
    // ---------------------------------------------------
    // Created : at 7/5/2021
    // Purpose: 
    // rate the #define variable naming 
    //
    // Parameters : file pointer, filename, the result[2]
    // Returns : same as fgetc after skipping the contents
    // Side-effects : no side effects

    char buffer[1024];//buffer for the line
    char chr;//the current char
    int index;//index for buffer
    char *sub;//for strstr
    int line_num;//the current line number
    while(1){
        index=0;
        while(chr!='#'){
            chr=fgetc_plus_fn_for_var(fp);
            if(feof(fp)) return 1;
        }
        while(chr!='\n' && chr!=EOF){
            buffer[index]=chr;
            index++;
            if(index>=1024) break;
            chr=fgetc_plus_fn_for_var(fp);
        }
        line_num=at_line(fp);
        if(chr=='\n') line_num--;
        if(index>=1024) continue;
        if(index==0) break;
        buffer[index]='\0';
        sub=strstr(buffer, "define");
        if(sub==NULL) continue;
        if(strlen(sub)==6) continue;
        chr = sub[6];
        if(chr!=' ' && chr!='\t') continue;
        sub = strchr(sub, chr);
        for(index=0; index<strlen(sub); index++){
            if(sub[index]!=' ' && sub[index]!='\t'){
                break;
            }
        }
        if(index==strlen(sub)) continue;
        rate_result[0]++;
        rate_result[1]++;
        for(index=index; index<strlen(sub); index++){
            if(sub[index]==' ' && sub[index]=='\t'){
                break;
            }
            if(sub[index]>='a' && sub[index]<='z'){
                flag_code(filename, line_num, "Defined identifiers should be all CAPS", get_this_line_code(fp, line_num));
                rate_result[0]--;
                break;
            }
        }
        if(feof(fp)) break;
    }
    return 1;
}

int pointer_name(char *dstn, char *src){
    int flag=0;
    int index=0;
    for(int i=0; i<strlen(src); i++){
        if(src[i]!='*') flag=1;
        if(flag) dstn[index++]=src[i];
    }
    dstn[index]='\0';
    return 1;
}

int before_comma(char *dstn, char *src){
    int index=0;
    for(int i=0; i<strlen(src); i++){
        if(src[i]==';' || src[i]==',') break;
        dstn[index++]=src[i];
    }
    dstn[index]='\0';
    return 1;
}

int rate_const_naming(FILE *fp, char *filename, int *result){
    if(filename ==NULL || fp==NULL) return 0;
    char token[100];
    char name[100];
    char var_name[100];
    int line_num;
    char *tmp;
    int record=0;
    int index=0;
    struct VAR_LIST *cur=NULL;
    char *buffer;
    char var_info[1024];
    while(!feof(fp)){
        record=ftell(fp);
        if(fget_token(token, 100, fp)==EOF) break;
        pointer_name(name, token);        
        if(strcmp(name, "const")!=0) continue;
        if(fget_token(token, 100, fp)==EOF) break;
        result[1]++;
        line_num=at_line(fp);
        if(in_list(token, types, type_number)==0){
            tmp=strchr(token, '=');
            line_num=at_line(fp);
            for(index=0; index<strlen(token); index++){
                if(token[index]=='=' || token[index]==',') index=strlen(token);
                if(token[index]>='a' && token[index]<='z'){
                    before_comma(var_name, token);
                    if(feof(fp)) fseek(fp, -1, SEEK_CUR);
                    sprintf(var_info, "VAR [%s] at line %d: %s", var_name, line_num, get_this_line_code(fp, line_num));
                    flag_code(filename, line_num, "Constant variable should be all CAPS", var_info);
                    break;
                }
            }
            if(index==strlen(token)) result[0]++;
        }
        else{
            fseek(fp, record, SEEK_SET);
            cur=get_var(fp);
            if(cur==NULL) continue;
            while(cur!=NULL){
                buffer=cur->cur->name;
                for(index=0; index<strlen(buffer); index++){
                    if(buffer[index]>='a' && buffer[index]<='z'){
                        before_comma(var_name, buffer);
                        if(feof(fp)) fseek(fp, -1, SEEK_CUR);
                        sprintf(var_info, "VAR [%s] at line %d: %s", var_name, line_num, get_this_line_code(fp, line_num));
                        flag_code(filename, line_num, "Constant variable should be all CAPS", var_info);
                        break;
                    }
                }
                if(index==strlen(buffer)) result[0]++;
                cur=cur->next;
            }
        }
    }
    return 1;
}
