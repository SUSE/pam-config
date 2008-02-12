/* Copyright (C) 2007, 2008 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@thkukuk.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
parse_config_make (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** parse_config_%s (%s): '%s'\n", this->name,
	    type2string (type), args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *key = strsep (&args, " \t");
      char *val;

      if (args)
	while (isspace ((int) *args))
	  ++args;

      if (key[0] == '/')
	{
	  /* this is the /path/ ... option */
	  if (opt_set->set_opt (opt_set, "option", strdup (key)) == FALSE)
	    print_unknown_option_error (this->name, key);
	}
      else if (NULL != (val = strchr (key, '=')))
	{
	  *val++='\0';
	  if (opt_set->set_opt (opt_set, key, strdup (val)) == FALSE)
	    print_unknown_option_error (this->name, key);
	}
      else if (opt_set->enable (opt_set, key, TRUE) == FALSE)
	print_unknown_option_error (this->name, key);
    }

  return 1;
}

static int
write_config_make (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    printf ("**** write_config_make (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != PASSWORD)
    return 0;

  fprintf (fp, "password\toptional\tpam_make.so\t");

  bool_option_t **cur_bool_opt = opt_set->bool_opts;
  string_option_t **cur_str_opt = opt_set->string_opts;

  while (*cur_bool_opt != NULL)
    {
      if ((strcmp ((*cur_bool_opt)->key, "is_enabled") != 0)
	  && ((*cur_bool_opt)->value == TRUE))
	fprintf (fp, "%s ", (*cur_bool_opt)->key);
      cur_bool_opt++;
    }

  while (*cur_str_opt != NULL)
    {
      if ((strcmp ((*cur_str_opt)->key, "empty") != 0)
	  && ((*cur_str_opt)->value))
	{
	  if (strcmp ((*cur_str_opt)->key, "option") == 0)
	    fprintf (fp, "%s", (*cur_str_opt)->value);
	  else
	    fprintf (fp, "%s=%s ", (*cur_str_opt)->key, (*cur_str_opt)->value);
	}
      cur_str_opt++;
    }

  fprintf (fp, "\n");

  return 0;
}

GETOPT_START_1(PASSWORD)
GETOPT_END_1(PASSWORD)

PRINT_ARGS("make")

/* ---- contruct module object ---- */
     DECLARE_BOOL_OPTS_3 (is_enabled, debug, nosetuid);
DECLARE_STRING_OPTS_3 (make, log, option);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_make = { "pam_make.so", opt_sets,
			       &parse_config_make,
			       &def_print_module,
			       &write_config_make,
			       &get_opt_set,
			       &getopt,
			       &print_args};
