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
parse_config_umask (pam_module_t * this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** parse_config_umask (%s): '%s'\n", type2string (type),
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
      else if (strcmp (cp, "silent") == 0)
	opt_set->enable (opt_set, "silent", TRUE);
      else if (strcmp (cp, "usergroups") == 0)
	opt_set->enable (opt_set, "usergroups", TRUE);
      else if (strncmp (cp, "umask=", 6) == 0)
	opt_set->set_opt (opt_set, "umask", &cp[6]);
      else
	print_unknown_option_error ("pam_umask.so", cp);
    }
  return 1;
}

static int
print_module_umask_type (pam_module_t * this, write_type_t type)
{
  printf ("%s:", type2string (type));
  option_set_t *opt_set = this->get_opt_set (this, type);
  for_each_bool_opt (opt_set, &print_bool_opt);
  printf ("\n");
  return TRUE;
}

static int
print_module_umask (pam_module_t * this)
{
  print_module_umask_type (this, SESSION);
  return TRUE;
}

static int
write_config_umask (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  const char *cp;

  if (debug)
    printf ("**** write_config_umask (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != SESSION)
    return 0;

  fprintf (fp, "session\toptional\tpam_umask.so\t");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  if (opt_set->is_enabled (opt_set, "silent"))
    fprintf (fp, "silent ");
  if (opt_set->is_enabled (opt_set, "usergroups"))
    fprintf (fp, "usergroups ");
  cp = opt_set->get_opt (opt_set, "umask");
  if (cp)
    fprintf (fp, "umask=%s ", cp);

  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_4 (is_enabled, debug, silent, usergroups);
DECLARE_STRING_OPTS_1 (umask);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_umask = { "pam_umask.so", opt_sets,
			       &parse_config_umask,
			       &print_module_umask,
			       &write_config_umask,
			       &get_opt_set};
