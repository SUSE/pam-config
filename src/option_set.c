#include "option_set.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
is_enabled (option_set_t *this, char *key)
{
  if (!this)
    return FALSE;
  bool_option_t **current_opt = this->bool_opts;
  while (NULL != *current_opt)
    {
      if (0 == strcmp ((*current_opt)->key, key))
	{
	  return (*current_opt)->value;
	}
      current_opt++;
    }
  /* XXX for debugging */
  if (strcmp (key, "debug") != 0 && strcmp (key, "nullok") != 0)
    {
       fprintf (stderr, "ERROR: is_enabled: key '%s' not found!\n", key);
       abort ();
    }
  return FALSE;
}

int
enable (option_set_t * this, char *key, int value)
{
  if (!this)
    return FALSE;
  bool_option_t **current_opt = this->bool_opts;
  while (NULL != *current_opt)
    {
      if (0 == strcmp ((*current_opt)->key, key))
	{
	  (*current_opt)->value = value;
	  return TRUE;
	}
      current_opt++;
    }
  return FALSE;
}

// string opt functions
char *
get_opt (option_set_t * this, char *key)
{
  if (!this)
    return NULL;
  string_option_t **current_opt = this->string_opts;
  while (NULL != *current_opt)
    {
      if (0 == strcmp ((*current_opt)->key, key))
	{
	  return (*current_opt)->value;
	}
      current_opt++;
    }
  /* XXX for debugging */
  if (strcmp (key, "debug") != 0 && strcmp (key, "nullok") != 0)
    {
       fprintf (stderr, "ERROR: get_opt: key '%s' not found!\n", key);
       abort ();
    }

  return NULL;
}

int
set_opt (option_set_t * this, char *key, char *value)
{
  if (!this)
    return FALSE;
  string_option_t **current_opt = this->string_opts;
  while (NULL != *current_opt)
    {
      if (0 == strcmp ((*current_opt)->key, key))
	{
	  (*current_opt)->value = value;
	  return TRUE;
	}
      current_opt++;
    }

  return FALSE;
}

void
print_bool_opt (bool_option_t * opt)
{
  /* is_enabled is internal key.  */
  if (strcmp (opt->key, "is_enabled") == 0)
    return;

  if (opt->value)
    printf (" %s", opt->key);
}

void
for_each_bool_opt (option_set_t * this, void (*action) (bool_option_t * opt))
{
  if (!this)
    return;
  bool_option_t **current_opt = this->bool_opts;
  while (NULL != *current_opt)
    {
      action (*current_opt);
      current_opt++;
    }
  return;
}

void
print_string_opt (string_option_t * opt)
{
  if (opt->value)
    printf (" %s=%s", opt->key, opt->value);
}

void
for_each_string_opt (option_set_t * this,
		     void (*action) (string_option_t * opt))
{
  if (!this)
    return;
  string_option_t **current_opt = this->string_opts;
  while (NULL != *current_opt)
    {
      action (*current_opt);
      current_opt++;
    }
  return;
}
