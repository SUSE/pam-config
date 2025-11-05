/* Copyright (C) 2021 SUSE LLC

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
write_config_systemd_home (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  char *opt;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
  {
    case AUTH:
      fprintf (fp, "auth\tsufficient\tpam_systemd_home.so");
      break;
    case ACCOUNT:
      fprintf (fp, "account\tsufficient\tpam_systemd_home.so");
      break;
    case PASSWORD:
      fprintf (fp, "password\tsufficient\tpam_systemd_home.so");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_systemd_home.so");
      break;
  }

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf(fp, " debug");
  if ((opt = opt_set->get_opt (opt_set, "suspend")))
    fprintf(fp, " suspend=%s",opt);

  fprintf(fp, "\n");
  return 0;

}

static int
parse_config_systemd_home (pam_module_t *this, char *args, write_type_t type)
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
      else if (strncmp (cp, "suspend=", 8) == 0)
	   opt_set->set_opt (opt_set, "suspend", strdup(&cp[8]));
      else
	   print_unknown_option_error ("pam_systemd_home.so", cp);
    }
  return 1;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("systemd_home")
PRINT_XMLHELP("systemd_home")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_1 (suspend);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_systemd_home = { "pam_systemd_home.so", opt_sets, helptext,
				     &parse_config_systemd_home,
				     &def_print_module,
				     &write_config_systemd_home,
				     &get_opt_set,
				     &getopt,
				     &print_args,
				     &print_xmlhelp,
				     NULL,
				     PAM_SYSTEMD_HOME_AUTH_PRIORITY,
				     PAM_SYSTEMD_HOME_ACCOUNT_PRIORITY,
				     PAM_SYSTEMD_HOME_PASSWORD_PRIORITY,
				     PAM_SYSTEMD_HOME_SESSION_PRIORITY
};
