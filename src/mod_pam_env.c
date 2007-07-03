/* Copyright (C) 2007 Thorsten Kukuk
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
parse_config_env (pam_module_t * this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** parse_config_env (%s): '%s'\n", type2string (type),
	    args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int) *args))
	  ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable (opt_set, "debug", TRUE);
      else if (strncmp (cp, "conffile=", 9) == 0)
	opt_set->set_opt (opt_set, "conffile", strdup (&cp[9]));
      else if (strncmp (cp, "envfile=", 8) == 0)
	opt_set->set_opt (opt_set, "envfile", strdup (&cp[8]));
      else if (strncmp (cp, "readenv=", 8) == 0)
	opt_set->set_opt (opt_set, "readenv", strdup (&cp[8]));
      else
	print_unknown_option_error ("pam_env.so", cp);
    }
  return 1;
}

static int
write_config_env (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  const char *cp;

  if (debug)
    printf ("**** write_config_env (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case AUTH:
      fprintf (fp, "auth\trequired\tpam_env.so\t");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_env.so\t");
      break;
    default:
      return 0;
    }

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");

  cp = opt_set->get_opt (opt_set, "conffile");
  if (cp)
    fprintf (fp, "conffile=%s ", cp);
  cp = opt_set->get_opt (opt_set, "envfile");
  if (cp)
    fprintf (fp, "envfile=%s ", cp);
  cp = opt_set->get_opt (opt_set, "readenv");
  if (cp)
    fprintf (fp, "readenv=%s ", cp);

  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_3 (conffile, envfile, readenv);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_env = {"pam_env.so", opt_sets,
			    &parse_config_env,
			    &def_print_module,
			    &write_config_env,
			    &get_opt_set};
