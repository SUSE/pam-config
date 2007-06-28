#include "option_set.h"

#include <string.h>
#include <stdio.h>

int
is_enabled( option_set_t *this,  char *key ){
  if( ! this ) return FALSE;
  bool_option_t **current_opt = this->bool_opts;
  while( NULL != *current_opt ){
    if( 0 == strcmp( (*current_opt)->key, key ) ){
      return (*current_opt)->value;
    }
    current_opt++;
  }
  return FALSE;
}

int
enable( option_set_t *this, char *key, int value ){
  if( ! this ) return FALSE;
  bool_option_t **current_opt = this->bool_opts;
  while( NULL != *current_opt ){
    if( 0 == strcmp( (*current_opt)->key, key ) ){
      (*current_opt)->value = value;
      return TRUE;
    }
    current_opt++;
  }
  return FALSE;
}

// string opt functions
char* get_opt ( option_set_t *this, char *key ){
  if( ! this ) return NULL;
  string_option_t **current_opt = this->string_opts;
  while( NULL != *current_opt ){
    if( 0 == strcmp( (*current_opt)->key, key ) ){
      return (*current_opt)->value;
    }
    current_opt++;
  }
  return NULL;
}

int set_opt ( option_set_t *this, char *key, char *value ){
  if( ! this ) return FALSE;
  string_option_t **current_opt = this->string_opts;
  while( NULL != *current_opt ){
    if( 0 == strcmp( (*current_opt)->key, key ) ){
      (*current_opt)->value = value;
      return TRUE;
    }
    current_opt++;
  }
  return FALSE;
}

void
print_bool_opt (bool_option_t *opt)
{
  if (opt->value)
     printf (" %s", opt->key);
}

void for_each_bool_opt( option_set_t *this, void (*action) (bool_option_t *opt) ){
  if( ! this ) return;
  bool_option_t **current_opt = this->bool_opts;
  while( NULL != *current_opt ){
    action( *current_opt );
    current_opt++;
  }
  return;
}
