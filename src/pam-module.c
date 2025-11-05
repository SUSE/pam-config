
/*
 * Copyright (C) 2007, 2008 - Thorsten Kukuk, Sven Schober
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pam-config.h"
#include "pam-module.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
def_print_module_type (pam_module_t *this, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return;

  printf ("%s:", type2string( type ) );
  for_each_bool_opt( opt_set,  &print_bool_opt );
  for_each_string_opt( opt_set, &print_string_opt );
  printf( "\n" );
}

int
def_print_module (pam_module_t *this)
{
  def_print_module_type (this, AUTH);
  def_print_module_type (this, ACCOUNT);
  def_print_module_type (this, PASSWORD);
  def_print_module_type (this, SESSION);

  return TRUE;
}

int
def_write_config( pam_module_t *this, enum write_type op __attribute__ ((unused)), FILE *fp __attribute__ ((unused)) ){
  printf( "default write module:\t%s\n", this->name );
  return 1;
}

pam_module_t* lookup( pam_module_t **module_list, const char *module ){
  while (*module_list != NULL) {
    if( 0 == strcmp( (*module_list)->name, module ) ){
      return *module_list;
    }
    module_list++;
  };
  return *module_list;
}

int
def_parse_config (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** def_parse_config [%s] (%s): '%s'\n", this->name,
	    type2string (type), args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  PARSE_CONFIG_OPTIONS

  return 1;
}

int
is_module_enabled (pam_module_t **module_list, const char *module,
		   write_type_t op)
{
  pam_module_t *mod = lookup (module_list, module);

  if (mod == NULL)
    return FALSE;

  option_set_t *opt_set = mod->get_opt_set (mod,op);

  return opt_set->is_enabled (opt_set, "is_enabled");
}

const char *
type2string (write_type_t wt)
{
  if( ACCOUNT == wt ) return "account";
  else if( AUTH == wt ) return "auth";
  else if( PASSWORD == wt ) return "password";
  else if( SESSION == wt ) return "session";
  else return "<unknown type>";
}

void
print_module_config (pam_module_t **module_list, const char *module)
{
  while (*module_list != NULL)
    {
      if (strcmp ((*module_list)->name, module) == 0)
        (*module_list)->print_module( (*module_list) );
      module_list++;
    }
}

void
print_module_args (pam_module_t **module_list)
{
  while (*module_list != NULL)
    {
      if ((*module_list)->print_args != NULL)
	(*module_list)->print_args (*module_list);
      module_list++;
    }
}

void
print_module_xmlhelp (pam_module_t **module_list)
{
  while (*module_list != NULL)
    {
      if ((*module_list)->print_xmlhelp != NULL)
	(*module_list)->print_xmlhelp (*module_list);
      module_list++;
    }
}

void
print_unknown_option_error (const char *module, const char *option)
{
  /* TRANSLATORS: first argument is name of a PAM module */
  fprintf (stderr, _("Unknown option for %s, ignored: '%s'\n"),
	   module, option);
}

option_set_t*
get_opt_set( pam_module_t *this, write_type_t op ){
  return (this->option_sets)[op];
}

/* return value: 0 for found, 1 for not found */
int
module_getopt (pam_module_t **module_list, const char *optarg,
	       global_opt_t *opt)
{
  char *work;
  char *name;
  char *arg;
  char *cp;

  if (optarg[0] != '-' || optarg[1] != '-')
    return 1;

  work = strdupa (&optarg[2]);

  name = work;

  arg = strchr (work, '=');
  if (arg)
    {
      *arg++ = '\0';
    }

  cp = strchr (&work[2], '-');
  if (cp)
    {
      *cp++ = '\0';
      work = cp;
    }
  else
    work = "";

  if (asprintf(&name, "pam_%s.so", name) < 0)
    return 1;

  if (debug)
    fprintf (stderr, "module=%s, option=%s, argument=%s\n", name, work, arg);

  while (*module_list != NULL)
    {
      int retval;

      if (strcmp ((*module_list)->name, name) == 0 &&
	  (*module_list)->getopt != NULL)
        {
	   retval = (*module_list)->getopt ((*module_list), work, arg, opt);
           if (retval == 0)
	     {
	       free (name);
	       return 0;
	     }
           else if (retval == 2) /* module not installed */
             {
               free (name);
               return 2;
             }
        }
      module_list++;
    }

  free (name);

  return 1;
}

void
debug_write_call (pam_module_t *this, enum write_type type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);
  int is_used = opt_set->is_enabled (opt_set, "is_enabled");

  printf ("**** write config for %s (%s, %s)\n",
	  this->name,
	  type2string (type),
	  is_used?"enabled":"disabled");
}

void configurable_module_free(configurable_module_t *sp) {
  free(sp->modname);
  free(sp->account_line);
  free(sp->auth_line);
  free(sp->password_line);
  free(sp->session_line);
  free(sp);
}