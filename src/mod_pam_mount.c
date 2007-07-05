/* Copyright (C) 2007 Sven Schober
   Author: Sven Schober <sschober@suse.de>

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
parse_config_mount (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_mount (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;
      
      if (opt_set->enable (opt_set, cp, TRUE) == FALSE){
	if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
	else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
	else
	  print_unknown_option_error ("pam_mount.so", cp);
      }
    }
  return 1;
}

static int
write_config_mount (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    printf ("**** write_config_mount (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
  {
    case AUTH:
      fprintf (fp, "auth\trequired\tpam_mount.so\t");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_mount.so\t");
      break;
    default:
      fprintf (stderr, _("Management group %s not supported by pam_mount.so"), type2string (op));
  }
  
  fprintf (fp, "\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_1( is_enabled );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_mount = { "pam_mount.so", opt_sets,
  &parse_config_mount,
  &def_print_module,
  &write_config_mount,
  &get_opt_set};
