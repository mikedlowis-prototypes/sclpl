#include "scanner.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void scanner_getline(scanner_t* p_scanner);
static void scanner_skip_ws(scanner_t* p_scanner);
static char scanner_current(scanner_t* p_scanner);
static bool scanner_oneof(scanner_t* p_scanner, const char* p_set);
static char* scanner_dup(scanner_t* p_scanner, size_t start_idx, size_t len);

scanner_t* scanner_new(char* p_prompt, FILE* p_file) {
    scanner_t* p_scanner = (scanner_t*)malloc(sizeof(scanner_t));
    p_scanner->p_line   = NULL;
    p_scanner->index    = 0;
    p_scanner->p_input  = p_file;
    p_scanner->p_prompt = p_prompt;
    return p_scanner;
}

char* scanner_read(scanner_t* p_scanner) {
    char* p_tok = NULL;
    scanner_skip_ws(p_scanner);
    if (!scanner_eof(p_scanner)) {
        if (scanner_oneof(p_scanner, "()[]{};,'\"")) {
            p_tok = scanner_dup(p_scanner, p_scanner->index, 1);
            p_scanner->index++;
        } else {
            size_t start =  p_scanner->index;
            while(!scanner_oneof(p_scanner," \t\r\n()[];,'\"")) {
                p_scanner->index++;
            }
            p_tok = scanner_dup(p_scanner, start, p_scanner->index - start);
        }
    }
    return p_tok;
}

bool scanner_eof(scanner_t* p_scanner)
{
    return (scanner_eol(p_scanner) && feof(p_scanner->p_input));
}

bool scanner_eol(scanner_t* p_scanner)
{
    bool ret = true;
    size_t index = p_scanner->index;
    char ch;
    while((NULL != p_scanner->p_line) && ('\0' != (ch = p_scanner->p_line[index]))) {
        if((' '!=ch) && ('\t'!=ch) && ('\r'!=ch) && ('\n'!=ch)) {
            ret = false;
            break;
        }
        index++;
    }
    return ret;
}

static void scanner_getline(scanner_t* p_scanner) {
    int c;
    size_t capacity = 8;
    size_t index    = 0;
    /* Reallocate and clear the line buffer */
    p_scanner->p_line = realloc(p_scanner->p_line, capacity);
    p_scanner->p_line[0] = '\0';
    p_scanner->index = 0;

    /* If we have not yet reached the end of the file, read the next line */
    if (!scanner_eof(p_scanner)) {
        if (NULL != p_scanner->p_prompt)
            printf("%s", p_scanner->p_prompt);
        while(('\n' != (c = fgetc(p_scanner->p_input))) && (EOF != c)) {
            if (index+2 == capacity) {
                capacity <<= 1u;
                p_scanner->p_line = realloc(p_scanner->p_line, capacity);
            }
            p_scanner->p_line[index++] = c;
        }
        p_scanner->p_line[index++] = (c == EOF) ? '\0' : c;
        p_scanner->p_line[index++] = '\0';
        p_scanner->index = 0;
    }
}

static void scanner_skip_ws(scanner_t* p_scanner) {
    /* If we haven't read a line yet, read one now */
    if (NULL == p_scanner->p_line)
        scanner_getline(p_scanner);
    while('\0' == scanner_current(p_scanner) || scanner_oneof(p_scanner, " \t\r\n")) {
        if ('\0' == scanner_current(p_scanner) && !scanner_eof(p_scanner)) {
            scanner_getline(p_scanner);
        } else {
            p_scanner->index++;
        }
    }
}

static char scanner_current(scanner_t* p_scanner) {
    return p_scanner->p_line[p_scanner->index];
}

static bool scanner_oneof(scanner_t* p_scanner, const char* p_set) {
    bool ret = false;
    size_t sz = strlen(p_set);
    for (size_t idx = 0; idx < sz; idx++) {
        if (scanner_current(p_scanner) == p_set[idx]) {
            ret = true;
            break;
        }
    }
    return ret;
}

static char* scanner_dup(scanner_t* p_scanner, size_t start_idx, size_t len) {
    char* p_str = (char*)malloc(len+1);
    memcpy(p_str, &(p_scanner->p_line[start_idx]), len);
    p_str[len] = '\0';
    return p_str;
}

