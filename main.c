#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"parser.h"
#include"output.h"
#include"rate_comment.h"
#include"rate_doc.h"
#include"rate_indentation.h"
#include"rate_modular.h"
#include"rate_testcases.h"
#include"rate_var.h"
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
// Programmer : Jiaxuan Chen
// Created : Jiaxuan Chen at 5/11/2021
// Purpose:
// The main program and the error handler
//
// Modifications:
// Initial Date Short Description 
// <none>

int error_dealer(int code){
    // ---------------------------------------------------
    // Created : at 5/11/2021 
    // Purpose:
    // handles all kind of error code
    //
    // Parameters : error code
    // Returns : NULL
    // Side-effects : exist the program

    printf("Error code is %d\n", code);
    if(code==-1){
        printf("Input is not valid\n");
    }
    else if(code==-2){
        printf("File not exists\n");
    }
    else if(code==-3){
        printf("Please run collect_all_types first\n");
    }
    exit(code);
}

int main(int argc, char *argv[]){
    // ---------------------------------------------------
    // Created : at 5/11/2021
    // Purpose: main function of the whole program
    // Parameters : command line arguments
    // Returns : 0
    // Side-effects : this program starts running, bugs might happen, so good luck everyone!

    int index=1;//index is where the filename arguments begins
    int *modes=malloc(MODENUM*sizeof(int));//the mode list to save which modes are on
    int tmp=0;//file number
    int *file_num=&tmp;//file number pointer
    mode_parser(modes, argv, argc);
    //find out if the mode is on for "-help", "-ssf", "-nodoc", "-notest", "-novars"
    if(modes[0]==1) return help_command();
    for(int i=0; i<MODENUM; i++) index+=modes[i];
    
    char **filenames = filenames_parser(file_num, argv, argc, index);
    //get all the wanted file names
    char **headernames = malloc(*file_num*sizeof(char*));
    //get all the .h file names
    char **allfiles = malloc(*file_num*2*sizeof(char*));
    //all the files name
    for (int i = 0; i<*file_num;i++){
        char *temp = ".h";//the header
        char *headname = malloc(strlen(filenames[i])+1);//the header name
        strncat(headname,filenames[i], strlen(filenames[i])-2);
        strcat(headname,temp);
        headernames[i] = headname;
    }

    for (int i = 0; i<*file_num;i++){
        allfiles[i] = filenames[i];
    }

    for (int i = 0; i<*file_num;i++){
        allfiles[i+*file_num] = headernames[i];
    }

    if(filenames==NULL){
        error_dealer(-1);
    }

    printf("\nParsed filenames are:\n");

    for(int i=0; i<*file_num*2; i++){
        printf("%s ", allfiles[i]);
    }
    printf("\n\n");
    create_output_files();
    collect_all_types(allfiles, *file_num*2);
    printf("Collected types from input are: \n");
    for(int i=0; i<type_number; i++){
        printf("%s, ", types[i]);
    }
    printf("\n");
    rate_comment(filenames, *file_num);
    if(modes[2]==0) rate_doc(filenames, *file_num);
    if(modes[3]==0) rate_testcases(filenames, *file_num);
    if(modes[4]==0) rate_var(filenames, allfiles, *file_num);
    rate_indentation(filenames, *file_num); 
    if(modes[1]==0) rate_modular(filenames, headernames, *file_num);
    printf("\n");
    printf("ends\n");
    give_output();
    free(filenames);
    free(modes);
    free(allfiles);
    free(headernames);
    return 0;
}
