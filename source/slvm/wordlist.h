/**
  @file wordlist.h
  @brief This module defines the list of built-in words for the interpreter.
  $Revision$
  $HeadURL$
  */
#ifndef WORDLIST_H
#define WORDLIST_H

typedef void (*codeword_t)(void);

typedef struct word_t {
    struct word_t* link;
    long flags;
    char* name;
    codeword_t codeword;
    long* code;
} word_t;

extern word_t* LatestWord;

#endif /* WORDLIST_H */
