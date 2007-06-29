#ifndef _OPTION_SET_H_
#define _OPTION_SET_H_ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define FALSE 0
#define TRUE 1

typedef struct {
  char *key;
  int value;
} bool_option_t;

typedef struct {
  char *key;
  char *value;
} string_option_t;

typedef struct option_set {
  bool_option_t **bool_opts;
  string_option_t **string_opts;
  // bool opt functions
  int (*is_enabled)( struct option_set *this,  char *key );
  int (*enable)( struct option_set *this, char *key, int value );
  // string opt functions
  char* (*get_opt) ( struct option_set *this, char *key );
  int (*set_opt) ( struct option_set *this, char *key, char *value );
} option_set_t;

int is_enabled ( option_set_t *this, char *key );
int enable ( option_set_t *this, char *key, int value );
char* get_opt ( struct option_set *this, char *key );
int set_opt ( struct option_set *this, char *key, char *value );
void for_each_bool_opt( struct option_set *this, void (*action) (bool_option_t *opt ) );
void print_bool_opt( bool_option_t *opt );
void for_each_string_opt( struct option_set *this, void (*action) (string_option_t *opt ) );
void print_string_opt( string_option_t *opt );
#endif
