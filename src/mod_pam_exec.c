/* Copyright (C) 2010 Thorsten Kukuk
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
parse_config_exec (pam_module_t *this, char *args, write_type_t type)
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

      if (opt_set->enable (opt_set, key, TRUE) == FALSE)
	{
	  if (NULL != (val = strchr (key, '=')))
	    {
	      *val++='\0';
	      if (opt_set->set_opt (opt_set, key, strdup (val)) == FALSE)
		print_unknown_option_error (this->name, key);
	    }
	  else
	    {
	      /* this is the command ... option */
	      const char *oldval =  opt_set->get_opt (opt_set, "option");

	      if (oldval != NULL)
		{
		  char *cp;

		  if (asprintf (&cp, "%s %s", oldval, key) < 0)
		    {
		      fprintf (stderr, _("ERROR: Out of memory.\n"));
		      exit (1);
		    }

		  if (opt_set->set_opt (opt_set, "option", cp) == FALSE)
		    {
		      print_unknown_option_error (this->name, key);
		      free (cp);
		    }
		}
	      else
		{
		  if (opt_set->set_opt (opt_set, "option", strdup (key)) == FALSE)
		    print_unknown_option_error (this->name, key);
		}
	    }
	}
    }

  return 1;
}

static int
write_config_exec (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case AUTH:
      fprintf (fp, "auth\toptional\tpam_exec.so\t");
      break;
    case ACCOUNT:
      fprintf (fp, "account\toptional\tpam_exec.so\t");
      break;
    case PASSWORD:
      fprintf (fp, "password\toptional\tpam_exec.so\t");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_exec.so\t");
      break;
  }

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

PRINT_ARGS("exec")
PRINT_XMLHELP("exec")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_5 (is_enabled, debug, expose_authtok, seteuid, quiet);
DECLARE_STRING_OPTS_2 (log, option);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{"", NULL, "pam_exec for password management"},
				       {NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_exec = { "pam_exec.so", opt_sets, helptext,
			      &parse_config_exec,
			      &def_print_module,
			      &write_config_exec,
			      &get_opt_set,
			      &getopt,
			      &print_args,
			      &print_xmlhelp,
			      NULL,
			      -1,
			      -1,
			      PAM_EXEC_PASSWORD_PRIORITY,
			      PAM_EXEC_SESSION_PRIORITY
};
