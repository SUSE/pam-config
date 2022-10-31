/* Copyright (C) 2011 Frederic Crozat
   Author: Frederic Crozat <fcrozat@suse.com>

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
#include <stdlib.h>

#include "pam-config.h"
#include "pam-module.h"

static int
write_config_systemd (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  char *opt;

  if (debug)
    debug_write_call (this, op);

  if (op != SESSION || !opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  fprintf (fp, "session\toptional\tpam_systemd.so");

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf(fp, " debug");
  if ((opt = opt_set->get_opt (opt_set, "kill_session_processes")))
    fprintf(fp, " kill-session-processes=%s",opt);
  if ((opt = opt_set->get_opt (opt_set, "kill_only_users")))
    fprintf(fp, " kill-only-users=%s",opt);
  if ((opt = opt_set->get_opt (opt_set, "kill_exclude_users")))
    fprintf(fp, " kill-exclude-users=%s",opt);
  if ((opt = opt_set->get_opt (opt_set, "controllers")))
    fprintf(fp, " controllers=%s",opt);
  if ((opt = opt_set->get_opt (opt_set, "reset_controllers")))
    fprintf(fp, " reset-controllers=%s",opt);

  fprintf(fp, "\n");
  return 0;

}

static int
parse_config_systemd (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** parse_config_%s (%s): '%s'\n", this->name,
	    type2string (type), args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");

      if (args)
	while (isspace ((int) *args))
	  ++args;

      if (strcmp (cp, "debug") == 0)
	   opt_set->enable (opt_set, "debug", TRUE);
      else if (strncmp (cp, "kill-session-processes=", 13) == 0)
	   opt_set->set_opt (opt_set, "kill_session_processes", strdup(&cp[13]));
      else if (strncmp (cp, "kill-only-users=", 16) == 0)
  	   opt_set->set_opt (opt_set, "kill_only_users", strdup (&cp[16]));
      else if (strncmp (cp, "kill-exclude-users=", 19) == 0)
  	   opt_set->set_opt (opt_set, "kill_exclude_users", strdup (&cp[19]));
      else if (strncmp (cp, "controllers=", 12) == 0)
  	   opt_set->set_opt (opt_set, "controllers", strdup (&cp[12]));
      else if (strncmp (cp, "reset-controllers=", 18) == 0)
  	   opt_set->set_opt (opt_set, "reset_controllers", strdup (&cp[18]));
      else
	   print_unknown_option_error ("pam_systemd.so", cp);
    }
  return 1;
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("systemd")
PRINT_XMLHELP("systemd")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_5 (kill_session_processes, kill_only_users, kill_exclude_users, controllers, reset_controllers);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_systemd = { "pam_systemd.so", opt_sets, helptext,
				     &parse_config_systemd,
				     &def_print_module,
				     &write_config_systemd,
				     &get_opt_set,
				     &getopt,
				     &print_args,
				     &print_xmlhelp};
