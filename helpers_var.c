#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include"output.h"
#include"main.h"
// Programmer : Jiaxuan Chen, Yuyan Chen 
// Created : at 7/1/2021
// Purpose:
// provide helpers about variables and functions for other files
//
// Modifications:
// Initial Date Short Description 
// <none>\

struct FUNC *get_next_function(FILE *fp){
    // ---------------------------------------------------
    // Created : at 5/26/2021 
    // Purpose:
    // get information of next function from a file
    //
    // Parameters : file pointer
    // Returns : a func struct
    // Side-effects : NULL

    if(fp==NULL) return NULL;
    char cur=0;//current char we are on
    int index1=0;//iterator for buffer
    int index2=0;//iterator for para_buffer
    int index3=0;//iterator for name of function
    int tmp;//interator for multiple use
    int start;//record the start of the function
    char buffer[1024];//store the function line
    char para_buffer[1024];//store the parameters
    char name[100];//store the name of the function
    int empty_para=0;//flag if the parameter is empty
    while(1){
        index1=0;
        index2=0;
        index3=0;
        tmp=0;
        cur=0;
        while(cur!='{' && !feof(fp)) {
            start=ftell(fp);
            cur=fgetc_plus(fp);
            buffer[index1++]=cur;
            if(cur==EOF) return NULL;
        }
        if(feof(fp)) return NULL;
        buffer[index1]='\0';
        for(tmp=strlen(buffer)-2; tmp>=0; tmp--){
            if(buffer[tmp]!=' ' || buffer[tmp]!='\n' || buffer[tmp]!='\t') break;
        }
        if(tmp<0 || buffer[tmp]!=')') continue;
        for(tmp=tmp; tmp>=0; tmp--){
            if(buffer[tmp]=='(') break;
        }
        if(tmp<0 || buffer[tmp]!='(') continue;
        if(buffer[tmp+1]==')') empty_para=1;
        else{
            for(int i=tmp; i<strlen(buffer); i++) {
                para_buffer[index2++]=buffer[i];
                if(buffer[i]==')') break;
            }
            para_buffer[index2]='\0';
        }
        tmp--;
        while(buffer[tmp]==' ') tmp--;
        while(buffer[tmp]!=' ' && tmp>=0) tmp--;
        tmp++;
        if(tmp<0) continue;
        while(buffer[tmp]=='*') tmp++;
        while(buffer[tmp]!='(' && buffer[tmp]!=' ') name[index3++]=buffer[tmp++];
        name[index3]='\0';
        break;
    }
    struct FUNC *func=malloc(sizeof(struct FUNC));
    //the function to be returned
    func->func_start=start+1;
    fseek(fp, start, SEEK_SET);
    fgetc_plus(fp);
    func->offset=ftell(fp)-start;
    func->name=strdup(name);
    if(empty_para) func->parameters=NULL;
    else{
        for(tmp=0; tmp<strlen(para_buffer); tmp++){
            if(para_buffer[tmp]!=' ') break;
        }
        if(tmp==strlen(para_buffer)) func->parameters=NULL;
        else func->parameters=strdup(para_buffer);
    }
    return func;
}

struct FUNC *get_next_declaration(FILE *fp){
    // ---------------------------------------------------
    // Created : at 7/14/2021 
    // Purpose:
    // get information of next function from a header file
    //
    // Parameters : file pointer
    // Returns : a func struct
    // Side-effects : NULL

    if(fp==NULL) return NULL;
    char cur=0;//current char we are on
    int index1=0;//iterator for buffer
    int index2=0;//iterator for para_buffer
    int index3=0;//iterator for name of function
    int tmp;//interator for multiple use
    int start;//record the start of the function
    char buffer[1024];//store the function line
    char para_buffer[1024];//store the parameters
    char name[100];//store the name of the function
    int empty_para=0;//flag if the parameter is empty
    while(1){
        index1=0;
        index2=0;
        index3=0;
        tmp=0;
        cur=0;
        while(cur!=';' && !feof(fp)) {
            start=ftell(fp);
            cur=fgetc_plus(fp);
            buffer[index1++]=cur;
            if(cur==EOF) return NULL;
        }
        if(feof(fp)) return NULL;
        buffer[index1]='\0';
        for(tmp=strlen(buffer)-2; tmp>=0; tmp--){
            if(buffer[tmp]!=' ')break;
        }
        if(tmp<0 || buffer[tmp]!=')') continue;
        for(tmp=tmp; tmp>=0; tmp--){
            if(buffer[tmp]=='(') break;
        }
        if(tmp<0 || buffer[tmp]!='(') continue;
        if(buffer[tmp+1]==')') empty_para=1;
        else{
            for(int i=tmp; i<strlen(buffer); i++) {
                para_buffer[index2++]=buffer[i];
                if(buffer[i]==')') break;
            }
            para_buffer[index2]='\0';
        }
        tmp--;
        while(buffer[tmp]==' ') tmp--;
        while(buffer[tmp]!=' ' && tmp>=0) tmp--;
        tmp++;
        if(tmp<0) continue;
        while(buffer[tmp]=='*') tmp++;
        while(buffer[tmp]!='(' && buffer[tmp]!=' ') name[index3++]=buffer[tmp++];
        name[index3]='\0';
        break;
    }
    struct FUNC *func=malloc(sizeof(struct FUNC));
    //the function to be returned
    func->func_start=start+1;
    fseek(fp, start, SEEK_SET);
    fgetc_plus(fp);
    func->offset=ftell(fp)-start;
    func->name=strdup(name);
    if(empty_para) func->parameters=NULL;
    else{
        for(tmp=0; tmp<strlen(para_buffer); tmp++){
            if(para_buffer[tmp]!=' ') break;
        }
        if(tmp==strlen(para_buffer)) func->parameters=NULL;
        else func->parameters=strdup(para_buffer);
    }
    return func;
}

int append_list(struct VAR_LIST *head, struct VAR *var){
    // ---------------------------------------------------
    // Created : at 7/1/2021
    // Purpose:
    // append the variable list by input var
    //
    // Parameters : head of the list and the var wanted to be appended
    // Returns : 1 for good
    // Side-effects : NULL

    if(head->cur==NULL) {
        head->cur=malloc(sizeof(struct VAR*));
        head->cur->name=strdup(var->name);
        head->cur->type=strdup(var->type);
        head->next=NULL;
        return 1;
    }
    struct VAR_LIST *cur=head;
    //current VAR LIST we are on
    while(cur->next!=NULL){
        cur=cur->next;
    }
    cur->next=malloc(sizeof(struct VAR_LIST*));
    cur->next->cur=malloc(sizeof(struct VAR*));
    cur->next->cur->name=strdup(var->name);
    cur->next->cur->type=strdup(var->type);
    cur->next->next=NULL;
    return 1;
}

int append_funclist(struct FUNC_LIST *head, struct FUNC *new){
    // ---------------------------------------------------
    // Created : at 7/14/2021
    // Purpose:
    // append the function list with a new function (avoiding duplicates)
    //
    // Parameters : head of the list and the new function to append
    // Returns : 0 for failure (duplicate), 1 for success
    // Side-effects : NULL
    if(head->name == NULL) {
        head->name=strdup(new->name);
        head->next = NULL;
        return 1;
    }
    struct FUNC_LIST *cur=head;
    if(strcmp(cur->name, new->name) == 0) return 0;
    while(cur->next != NULL) {
        cur = cur->next;
        if(strcmp(cur->name, new->name) == 0) return 0;
    }

    cur->next = malloc(sizeof(struct FUNC_LIST*));
    cur->next->name=strdup(new->name);
    cur->next->next = NULL;
    return 1;
    
}

int contains_funclist(struct FUNC_LIST *head, struct FUNC *key) {
    // ---------------------------------------------------
    // Created : at 7/15/2021
    // Purpose:
    // checks if the function list contains the key
    //
    // Parameters : head of the list and the key function to search
    // Returns : 0 -> false, 1 -> true
    // Side-effects : NULL

    struct FUNC_LIST *cur=head;
    if(cur->name == NULL) return 0;
    if(strcmp(cur->name, key->name) == 0) return 1;
    while(cur->next != NULL) {
        cur = cur->next;
        if(strcmp(cur->name, key->name) == 0) return 1;
    }
    return 0;
}

int contains_varlist(struct VAR_LIST *head, struct VAR *key) {
    // ---------------------------------------------------
    // Created : at 7/16/2021
    // Purpose:
    // checks if the var list contains the key
    //
    // Parameters : head of the list and the key value to search
    // Returns : 0 -> false, 1 -> true
    // Side-effects : NULL

    if(head == NULL) return 0;
    if(head->cur == NULL) return 0;

    if(strcmp(head->cur->name, key->name) == 0 && strcmp(head->cur->type, key->type) == 0)
        return 1;
    while(head->next != NULL) {
        head = head->next;
        if(strcmp(head->cur->name, key->name) == 0 && strcmp(head->cur->type, key->type) == 0)
            return 1;
    }
    
    return 0;
}

struct VAR_LIST *get_var(FILE *fp){
    // ---------------------------------------------------
    // Created : at 7/1/2021
    // Purpose:
    // get and parse the statement that declares variable
    // not that if a statement declares multiple variable it will be formed in a linked list
    // Parameters : file pointer
    // Returns : the linked list
    // Side-effects : NULL

    char type[100];//type of the var
    char *statement;//the whole declaration statement
    char *tmp;//pointer for strstr and strchr
    char *token;//tokenized token from statement
    struct VAR_LIST *head=NULL;//head of the list
    struct VAR *var=NULL;//an element of the list
    char cur;//current char we are on
    int start=0;//start of the statement from ftell
    int size;//size of the statement
    int flag1=0;//flag to avoid () in the statement
    int flag2=0;//flag to avoid {} in the statement
    int index=0;//iterator for name 
    int line_num;//the line number of the file we are on
    while(1){
        cur = 0;
        size = 0;
        while(in_list(type, types, type_number)==0){
            if(fget_token(type, 100, fp)==EOF){
                return NULL;
            }
            if(strcmp("typedef", type)==0){
                while(cur!=';' && cur!=EOF && cur!='{'){
                    cur = fgetc_plus_fn(fp);
                }
                cur=0;
            }
        }
        start = ftell(fp);
        while(cur!=';' && cur!=EOF && cur!='{'){
            cur = fgetc_plus_fn(fp);
            if(cur=='='){
                while(cur!=';' && cur!=',' && cur!=EOF){
                    cur=fgetc_plus_fn(fp);
                    size++;
                }
            }
            size++;
        }
        if(cur==EOF) return NULL;
        if(cur=='{' || cur=='(') {
            type[0]='\0';
            continue;
        }
        break;
    }
    statement = malloc(size*sizeof(char));
    fseek(fp, start, SEEK_SET);
    flag1=0;
    flag2=0;
    while(index<size){
        cur = fgetc_plus_fn(fp);
        if(cur=='('){
            flag1++;
            if(flag1==1){
                statement[index]='(';
                index++;
            }
            continue;
        }
        if(cur==')'){
            flag1--;
            if(flag1==0){
                statement[index]=')';
                index++;
            }
            continue;
        }
        if(cur=='{'){
            flag2++;
            continue;
        }
        if(cur=='}'){
            flag2--;
            continue;
        }
        if(flag1==0 && flag2==0){
            statement[index]=cur;
            index++;
        }
        if(cur==';' || cur==EOF){
            statement[index]='\0';
            break;
        }
    }
    tmp = strchr(statement, '=');
    if(tmp==NULL){
        tmp = strchr(statement, '(');
        if(tmp!=NULL){
            return NULL;
        }
    }//declaration of function at beginning is not a variable
    tmp = strchr(statement, ',');
    //check how many variables are declared in this statement
    if(tmp == NULL){
        head = malloc(sizeof(struct VAR_LIST*));
        head->cur=malloc(sizeof(struct VAR *));
        head->cur->type = strdup(type);
        tmp = strchr(statement, '=');
        size = 0;
        if(tmp == NULL){
            head->cur->name = malloc(strlen(statement)*sizeof(char)+1);
        } else{
            head->cur->name = malloc(strlen(statement) - strlen(tmp)+1);
        }
        for(int i=0; i<strlen(statement); i++){
            if(statement[i]=='=') break;
            if(statement[i]!=' ' && statement[i]!=';' && statement[i]!='*') {
                head->cur->name[size]=statement[i];
                size++;
            }
            if(strstr(head->cur->name, "const")!=NULL && statement[i]==' '){
                head->cur->name[size]=' ';
                size++;
            }
            if(strstr(head->cur->name, "const ")!=NULL){
                size=0;
            }
        }
        head->cur->name[size]='\0';
        head->next=NULL;
    }
    else{
        token = strtok(statement, ",");
        head = malloc(sizeof(struct VAR_LIST*));
        head->cur=NULL;
        head->next=NULL;
        var=malloc(sizeof(struct VAR*));
        var->name=NULL;
        var->type=NULL;
        while(1){
            if(var->type==NULL){
                var->type=strdup(type);
            }else {
                var->type=realloc(var->type, strlen(type)*sizeof(char));
                strcpy(var->type, type);
            }
            tmp = strchr(token, '='); 
            if(tmp == NULL){
                if(var->name==NULL){
                    var->name=malloc((strlen(token)+1)*sizeof(char));
                }else {
                    var->name=realloc(var->name, (strlen(token)+1)*sizeof(char));
                }
            }else{
                if(var->name==NULL){
                    var->name=malloc((strlen(token) - strlen(tmp)+1)*sizeof(char));
                }else {
                    var->name=realloc(var->name, (strlen(token) - strlen(tmp)+1)*sizeof(char));
                }
            }
            size=0;
            var->name[0]='\0';
            for(int j=0; j<strlen(token); j++){
                if(token[j]=='=') break;
                if(token[j]!=' ' && token[j]!=';' && token[j]!='*') {
                    var->name[size]=token[j];
                    size++;
                }
                if(strstr(var->name, "const")!=NULL && token[j]==' '){
                    var->name[size]=' ';
                    size++;
                }
                if(strstr(var->name, "const ")!=NULL){
                    size=0;
                }
            }
            var->name[size]='\0';
            append_list(head, var);
            token=strtok(NULL, ",");
            if(token==NULL) break;
        }
    }
    free(statement);
    if(var!=NULL) free(var);
    return head;
}
