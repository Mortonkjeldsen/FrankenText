/**
 * @file main.c
 * @author Morten Kjeldsen (s255531@dtu.dk)
 * @brief Splits the book Frankenstein by Mary Shelley into tokens, then
 * generates and prints random sentences using the generated tokens.
 * @date 2025-11-04
 *
 * @disclaimer: This program was coded using a template provided as part of an
 * assignment for the DTU course Basic C Programming. Only the codeblocks
 * following // YOUR CODE HERE were added by me. Ai was used to help generate
 * ideas.
 *
 */
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_COUNT 15000
#define MAX_SUCCESSOR_COUNT MAX_WORD_COUNT / 2

char book[] = {
#embed "pg84.txt" /// Stores the content of the file as an array of chars.
    , '\0'};      /// Makes `book` a string.

/// Array of tokens registered so far.
/// No duplicates are allowed.
char *tokens[MAX_WORD_COUNT];
/// `tokens`'s current size
size_t tokens_size = 0;

/// Array of successor tokens
/// One token can have many successor tokens. `succs[x]` corresponds to
/// `token[x]`'s successors.
/// We store directly tokens instead of token_ids, because we will directly
/// print them. If we wanted to delete the book, then it would make more sense
/// to store `token_id`s
char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];
/// `succs`'s current size
size_t succs_sizes[MAX_WORD_COUNT];

/// Overwrites non-printable characters in `book` with a space.
/// Non-printable characters may lead to duplicates like
/// `"\xefthe" and "the"` even both print `the`.
void replace_non_printable_chars_with_space() {
  // YOUR CODE HERE
  for (size_t i = 0; i < strlen(book); ++i) {
    if (!isprint(book[i])) {
      book[i] = ' ';
    }
  }
}

/// Returns the id (index) of the token, creating it if necessary.
///
/// Returns token id if token exists in \c tokens, otherwise creates a new entry
/// in \c tokens and returns its token id.
///
/// \param token token to look up (or insert)
/// \return Index of `token` in \c tokens array
size_t token_id(char *token) {
  size_t id;
  for (id = 0; id < tokens_size; ++id) {
    if (strcmp(tokens[id], token) == 0) {
      return id;
    }
  }
  tokens[id] = token;
  ++tokens_size;
  return id;
}

/// Appends the token \c succ to the successors list of \c token.
void append_to_succs(char *token, char *succ) {
  size_t *next_empty_index_ptr = &succs_sizes[token_id(token)];

  if (*next_empty_index_ptr >= MAX_SUCCESSOR_COUNT) {
    printf("Successor array full.");
    exit(EXIT_FAILURE);
  }

  succs[token_id(token)][(*next_empty_index_ptr)++] = succ;
}

/// Creates tokens on \c book and fills \c tokens and \c succs using
/// the functions \c token_id and \c append_to_succs.
void tokenize_and_fill_succs(char *delimiters, char *str) {
  // YOUR CODE HERE
  char *saveptr = NULL;
  char *prev_token = NULL;
  char *token = strtok_r(str, delimiters, &saveptr);
  while (token) {
    size_t id = token_id(token);
    if (prev_token) {
      append_to_succs(prev_token, token);
    }
    prev_token = token;
    token = strtok_r(NULL, delimiters, &saveptr);
  }
}

/// Returns last character of a string
char last_char(char *str) {
  // YOUR CODE HERE
  size_t len = strlen(str);
  if (len == 0) {
    return '\0';
  }
  return str[len - 1];
}

/// Returns whether the token ends with `!`, `?` or `.`.
bool token_ends_a_sentence(char *token) {
  // YOUR CODE HERE
  char c = token[strlen(token) - 1];
  return c == '.' || c == '!' || c == '?';
}

/// Returns a random `token_id` that corresponds to a `token` that starts with a
/// capital letter.
/// Uses \c tokens and \c tokens_size.
size_t random_token_id_that_starts_a_sentence() {
  // YOUR CODE HERE
  size_t id;
  do {
    id = rand() % tokens_size;
  } while (!isupper(tokens[id][0]));
  return id;
}

/// Generates a random sentence using \c tokens, \c succs, and \c succs_sizes.
/// The sentence array will be filled up to \c sentence_size-1 characters using
/// random tokens until:
/// - a token is found where \c token_ends_a_sentence
/// - or more tokens cannot be concatenated to the \c sentence anymore.
/// Returns the filled sentence array.
///
/// @param sentence array what will be used for the sentence.
//
//                  Will be overwritten. Does not have to be initialized.
/// @param sentence_size
/// @return input sentence pointer
char *generate_sentence(char *sentence, size_t sentence_size) {
  size_t current_token_id = random_token_id_that_starts_a_sentence();
  char *token = tokens[current_token_id];

  sentence[0] = '\0';
  strcat(sentence, token);
  if (token_ends_a_sentence(token))
    return sentence;

  // Calculated sentence length for the next iteration.
  // Used to stop the loop if the length exceeds sentence size
  size_t sentence_len_next;
  // Concatenates random successors to the sentence as long as
  // `sentence` can hold them.
  do {
    // YOUR CODE HERE
    size_t succ_count = succs_sizes[current_token_id];
    if (succ_count == 0)
      break; // No successors -> stop
    size_t succ_index = rand() % succ_count;
    char *next_token = succs[current_token_id][succ_index];

    size_t needed = strlen(next_token) + 1;
    sentence_len_next = strlen(sentence) + needed;
    if (sentence_len_next >= sentence_size)
      break; // Sentence would get to big -> stop

    strcat(sentence, " ");
    strcat(sentence, next_token);
    if (token_ends_a_sentence(next_token))
      break; // Token ends a sentence -> stop
    current_token_id = token_id(next_token);
  } while (sentence_len_next < sentence_size - 1);
  return sentence;
}

int main() {
  replace_non_printable_chars_with_space();

  char *delimiters = " \n\r";
  tokenize_and_fill_succs(delimiters, book);

  char sentence[1000];
  srand(time(nullptr)); // Be random each time we run the program

  // Generate sentences until we find a question sentence.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '?');
  puts(sentence);
  puts("");

  // Initialize `sentence` and then generate sentences until we find a sentence
  // ending with an exclamation mark.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '!');
  puts(sentence);
}