#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rate_indentation.h"
#include "helpers_file.h"
#include "helpers_type.h"
#include "helpers_var.h"
#include "helpers.h"
#include "output.h"

// Programmer : Emile Marty
// Created : at 2021-07-01
// Purpose:
// Rates the program on Indentation rules
//
// Modifications:
// Initial Date Short Description
// <none>

char nextChar(FILE *fp)
{
    // ---------------------------------------------------
    // Created : 5/21/2021
    // Purpose:
    // skip the contents between ' ', " ", /**/ and after //
    // Parameters : file pointer
    // Returns : same as fgetc after skipping the contents
    // Side-effects : move the file pointer by one
    int cur, next;

    cur = fgetc(fp);
    if (cur == EOF)
        return EOF;
    next = fgetc(fp);
    if (feof(fp))
        return cur;
    fseek(fp, -1, SEEK_CUR);
    if (cur == '\r' || cur == '\n')
        return cur;

    // skip character
    if (cur == '\'')
    {
        if (next == '\\')
        {
            for (int i = 0; i < 4; i++)
                cur = fgetc(fp);
            return cur;
        }
        else
        {
            for (int i = 0; i < 3; i++)
                cur = fgetc(fp);
            return cur;
        }
    }

    // skip string
    if (cur == '\"')
    {
        while (1)
        {
            cur = fgetc(fp);
            if (cur == '\"')
                return nextChar(fp);
        }
    }

    // skip single-line comment
    if (cur == '/' && next == '/')
    {
        fseek(fp, 1, SEEK_CUR);
        while (1)
        {
            cur = fgetc(fp);
            if (cur == '\r' || cur == '\n' || cur == EOF)
                return cur;
        }
    }

    // skip multi-line comment
    if (cur == '/' && next == '*')
    {
        fseek(fp, 1, SEEK_CUR);
        while (1)
        {
            cur = fgetc(fp);
            next = fgetc(fp);
            if (feof(fp))
                return cur;
            fseek(fp, -1, SEEK_CUR);
            if (cur == '*' && next == '/')
            {
                fseek(fp, 1, SEEK_CUR);
                return nextChar(fp);
            }
        }
    }
    return cur;
}

int rate_indentation(char **filenames, int file_num)
{
    // ---------------------------------------------------
    // Created : Emile Marty
    // Purpose: Main rating function
    //
    //
    // Parameters : list of filenames to rate, size of the list
    // Returns : 0 -> failure, 1 -> success
    // Side-effects : Sends the indentation summary to output

    printf("rate_indentation called\n");
    int score = 0;
    int style = -1; // 0 -> spaces  1 -> tabs

    for (int i = 0; i < file_num; i++)
    {
        char *fname = filenames[i];
        FILE *fp = fopen(fname, "r");
        if (fp == NULL)
            continue;

        int indent = 0;

        char current = nextChar(fp);
        while (current != EOF)
        {
            if (current == '{')
            {
                indent++;
                char next = nextChar(fp);
                while (next == ' ') {
                    next = nextChar(fp);
                }
                if (next != '\r' && next != '\n')
                {              
                    score--;
                    flag_code(fname, at_line(fp), "This line does not follow the correct bracket formatting rules.", "Rule 2");
                    break;
                }
                fseek(fp, -1, SEEK_CUR);
            }
            if (current == '\n' && indent > 0)
            {
                if (style == -1)
                {
                    char next = fgetc(fp);
                    if (next == ' ')
                        style = 0;
                    else if (next == '\t')
                        style = 1;
                    else
                    {
                        score--;
                        flag_code(fname, at_line(fp), "Indentation expected after new line in nested code.", "Rule 1");
                        break;
                    }
                    fseek(fp, -1, SEEK_CUR);
                }

                int code = 0;
                if (style == 0)
                {
                    for (int j = 0; j < indent * 4; j++)
                    {
                        char next = fgetc(fp);
                        if (next != ' ')
                        {
                            if (j == 4 * (indent - 1) && next == '}')
                                indent--;
                            else if (next == '\t')
                                code = 1;
                            else
                                code = 2;
                            break;
                        }
                    }
                }
                else if (style == 1)
                {
                    for (int j = 0; j < indent; j++)
                    {
                        char next = fgetc(fp);
                        if (next != '\t')
                        {
                            if (j == indent - 1 && next == '}')
                                indent--;
                            else if (next == ' ')
                                code = 1;
                            else
                                code = 2;
                            break;
                        }
                    }
                }

                if (code == 1)
                {
                    flag_code(fname, at_line(fp), "Indentation style (spaces/tabs) must be consistent across all files. Expected: tabs", "Rule 4");
                    score--;
                    break;
                }
                if (code == 2)
                {
                    flag_code(fname, at_line(fp), "Code must be indented with 4 spaces or 1 tab per nest.", "Rule 1");
                    score--;
                    break;
                }

                char next = fgetc(fp);
                if (next == ' ' || next == '\t')
                {
                    flag_code(fname, at_line(fp), "All nested statements are indented to the exact same column.", "Rule 3");
                }
                fseek(fp, -1, SEEK_CUR);
            }
            current = nextChar(fp);
        }

        score++;
        fclose(fp);
    }

    fill_indentation_summary(score, file_num);
    return 1;
}
