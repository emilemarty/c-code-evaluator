#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"helpers_file.h"
#include"helpers_type.h"
#include"helpers_var.h"
#include"helpers.h"
#include "output.h"


// Programmer : Yuyan Chen
// Created : at 05/20/2021
// Purpose:
// rate built-in test cases
// the rules are as follows:
// Rule 1: Every .c file must have a testFILENAME() function.
// Rule 2: The .c file with the main() function must have a test() function. In other words, this is an exception to rule 1. The function name is only the word test().
// Rule 3: The test() function must call every testFILENAME() function at least once.
// Rule 4: The main() function must have a call to the test() function within an if-statement. It is assumed (not tested) that there is a way to activate this test (maybe through a switch or a menu selection).

// Modifications:
// 05/20/2021 Created 
// <none>



char *get_filename(char *filename){
    // ---------------------------------------------------
    // Created : Yuyan Chen 5/25/2021
    // Purpose: 
    // clear the path of the filename
    //
    // Parameters : a string
    // Returns : a string "FILENAME.c"
    // Side-effects : None
    char *token; // to store the first token
    char *temp; // to store the duplicate of filename 
    char *next; // to store tokens other than the first token

    temp = strdup(filename);
    token = strtok(temp, "/");
    while (1)
    {
        next = strtok(NULL, "/");
        if (next != NULL) token = next;
        else break;
    }

    return token;
}


char fgetc_testcases(FILE *fp){
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
    if (cur == '\n') return fgetc_testcases(fp);

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
            if (cur == '\"')  return fgetc_testcases(fp);
        }
    }

    // skip single-line comment
    if (cur == '/' && next == '/') {
        fseek(fp, 1, SEEK_CUR);
        while(1){
            cur = fgetc(fp);
                if (cur == '\n') return fgetc_testcases(fp);
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

    // skip space
    if (cur == ' '){
        return fgetc_testcases(fp);
    }
    return cur;
}


int rate_testcases(char **filenames, int file_num){
    // ---------------------------------------------------
    // Created : Yuyan Chen 05/24/2021
    // Purpose:
    // rating the testcases of the input files 
    //
    // Parameters : filenames and the number of them
    // Returns : 1 for good
    // Side-effects : no side effects

    /*after you finish evaluate the files
    rememeber to use function fill_testcases_summary to record the summary of your evaluation
    and use flag_code to flag the improper code
    these two functions are in output.c, check them out! 
    and modify it if you want!*/
    
    char *fn; // function name 
    char **testFNs = calloc(1,sizeof(char*)); // to store all testFILENAME()functions
    int testFN_num = 0; // iniitialize the number of testFILENAME functions
    int main_index = -1; // initialize the index of the file with main()
    int hits = 0; // initialize hits to 0 for fill_testcases_summary()
    int nums = file_num; // initialize nums to the number of input files
    int main_count = 0; // initialize the number of file(s) with main() to 0
    int main_start = -1; // to store the starting position of main()
    int main_offset = -1; // to store the length of main()
    int test_start = -1; // to store the starting position of test()
    int test_offset = -1; // to store the length of test()
    int pass_rule3 = 1; // 1 if the input files pass rule3, 0 otherwise
    int pass_rule4 = 0; // 1 if the input files pass rule4, 0 otherwise
    int has_testFN = 0, has_test = 0, has_main =0;
    struct FUNC *f; // to temporarily store a function 
 

    printf("rate_testcases called\n");
    
    for (int n = 0; n < file_num; n++){
        has_testFN = 0;
        has_test = 0;
        has_main = 0;
        fn = filenames[n];
        FILE *fp = fopen(fn, "rb"); 

       
        // generate "testFILENAME"
        char *upper = strdup(get_filename(fn)); // to store FILENAME
        char *testFN = calloc((strlen("test") + strlen(upper) -1),sizeof(char)); // to store "testFILENAME"
        to_upper(upper);
        strcat(testFN, "test");
        strncat(testFN, upper, strlen(upper)-2);
      
        
        // check if the file contains a main() function
        while(1){
            f = get_next_function(fp);
            if (f==NULL) break;
            if (strcmp(f->name, "main") ==0){
                main_index = n;
                has_main = 1;
                main_count += 1;
            }
            if (strcmp(f->name, "test") == 0){
               has_test = 1;
            }
            if (strcmp(f->name, testFN) ==0) has_testFN = 1;          
        }

        // check Rule1: if the file contains testFILENAME()
        if (!has_main){
            if (has_testFN){
                testFN_num += 1;
                testFNs = realloc(testFNs, testFN_num * sizeof(char*));
                testFNs[testFN_num-1] = testFN;
                hits += 1;
            }
            else{
                // error message: missing testFILENAME() function
                char *error = calloc((strlen(testFN) + strlen("() missing") + 1),sizeof(char)); // error message
                strcat(error, testFN);
                strcat(error, "() missing");
                flag_code(filenames[n], 0 ,error, "Not Applicable");       
            } 
        }

        // check Rule2
        else{
            if (!has_test){
            flag_code(filenames[n], 0 ,"test() missing", "Not Applicable");  
            }
        }

        fclose(fp);
    }
    

    /* no main() */
    if (main_count == 0) {
        flag_code("Not Applicable", 0 ,"main() missing", "Not Applicable"); 
    }
     
    if (main_count > 1) {
        flag_code("Not Applicable", 0 ,"more than one main()", "Not Applicable");
    }
    
    int line_test = -1;
    int line_main = -1;

    if (main_count == 1) {
        FILE *fp = fopen(filenames[main_index], "rb");
       
        while (1){     
            f = get_next_function(fp); 
            if (f==NULL) break;
          
            if (strcmp(f->name, "main") ==0){
                main_start = f->func_start;
                main_offset = f->offset;
                line_main = at_line(fp);
            }
            if (strcmp(f->name, "test") == 0){
                test_start = f->func_start;
                test_offset = f->offset;
                line_test = at_line(fp);
            }
        }


        /* check Rule 3: : the test function must call every testFILENAME() function at least once
         get the contents of test() */

        if (test_offset != -1){
            fseek(fp, test_start, SEEK_SET);
            char test_c; // to store each character
            int test_i =  0; // count the characters inside the function
            char *buffer_test = calloc(test_offset , sizeof(char)); // to store characters inside the test()
            int count = 0;

            while(1){
                test_c = fgetc_testcases(fp);
                buffer_test[test_i++] = test_c;
                if(test_i > test_offset) break; // the end of the main() function
            }
            buffer_test[test_i] = 0; // set a NULL character at the end of buffer
      
         
            /* check if every testFILENAME() function is called at least once */
            for (int i = 0; i < testFN_num; i++){
                char *str1 = calloc((strlen(testFNs[i]) + 2),sizeof(char)); // to store testFILENAME(
                strcat(str1, testFNs[i]);
                strcat(str1, "(");
                if (strstr(buffer_test, str1) == NULL){
                    char *error = malloc((strlen(testFNs[i]) + strlen("() is not called") + 1)*sizeof(char)); // error message
                    strcat(error, testFNs[i]);
                    strcat(error, "() is not called");
                    flag_code(filenames[i], line_test ,error==NULL? "NULL":error, "test ()");  
                    pass_rule3 = 0;
                    break;
                }
                free(str1);
            }

            free(buffer_test);
        }
        
      
        /* check Rule 4: The main() function must have a call 
            to the test() function within an if-statement.
            It is assumed (not tested) that there is a way 
            to activate this test (maybe through a switch or a menu selection).*/

        fseek(fp, main_start, SEEK_SET);
        int main_i = 0; // count the characters inside the function
        char cur; // to store each character
        char* buffer_main = calloc(main_offset, sizeof(char)); 
        while(1){
            cur = fgetc_testcases(fp);
            buffer_main[main_i++] = cur;
            if(main_i > main_offset) break; // the end of the main() function
        }
        buffer_main[main_i] = '\0'; // set a NULL character at the end of buffer
       
        char temp[main_i]; // to temporarily store the content inside a if block
        int brackets; // +1 if the current character is { , -1 if the current character is }
        int semicolon; // +1 if the current character is ;
        char *str3 = "test("; // to store test(

       
        

        /*
        format 1: if (...) {...}
        format 2: if (...) ...;           
        */
        for (int j = 0; j < main_i; j++){
            if (buffer_main[j] == 'i' && buffer_main[j+1] == 'f'){
                brackets = 0;
                semicolon = 0;
                int index = 0; // the index of a character inside temp
                while(1){
                    temp[index++] = buffer_main[j++];
                    if (buffer_main[j] == '{') brackets += 1;
                    if (buffer_main[j] == '}') brackets -= 1;
                    if (buffer_main[j] == ';') semicolon = 1;
                    // if semicolon == 1 and bracketes == 0, the end of the if block is reached 
                    if (semicolon && !brackets){
                        temp[index] = 0;
                        break;
                    }
                }
                if (strstr(temp, str3) != NULL){
                    pass_rule4 = 1;
                    break;
                }     
            }
        }

        free(buffer_main);
        if(!pass_rule4){
            char *error = "The main() function must have a call to the test() function within an if-statement."; // error message
            flag_code(filenames[main_index], line_main, error==NULL? "NULL":error , "main ()"); 
        } 

        fclose(fp);
     }
    
    if (pass_rule3 && pass_rule4) hits += 1;
    fill_testcases_summary(hits, nums);
    free(testFNs);
    return 1;
}



