/**
  @file wordlist.h
  @brief This module defines the list of built-in words for the interpreter.
  $Revision$
  $HeadURL$
  */
#ifndef WORDLIST_H
#define WORDLIST_H

typedef void (*codeword_t)(long const*);

typedef struct word_t {
    struct word_t const* link;
    long const flags;
    char const* name;
    codeword_t const codeword;
    long const* code;
} word_t;

extern word_t const* LatestWord;

#endif /* WORDLIST_H */
