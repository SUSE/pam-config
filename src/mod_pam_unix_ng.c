/* Copyright (C) 2007, 2008 Sven Schober
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
write_config_unix_ng (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_pwquality;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_pwquality = is_module_enabled (common_module_list, "pam_pwquality.so" , op);

  switch (op)
  {
    case AUTH:
      fprintf (fp, "auth\trequired\tpam_unix_ng.so\t");
      if (!opt_set->is_enabled (opt_set, "use_first_pass") &&
          !opt_set->is_enabled (opt_set, "try_first_pass"))
        opt_set->enable (opt_set, "try_first_pass", TRUE);
      break;
    case ACCOUNT:
      fprintf (fp, "account\trequired\tpam_unix_ng.so\t");
      break;
    case PASSWORD:
      fprintf (fp, "password\trequired\tpam_unix_ng.so\t");
      if (with_pwquality)
	fprintf (fp, "use_authtok ");
      break;
  case SESSION:
    fprintf (fp, "session\trequired\tpam_unix_ng.so\t");
    break;
  }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("unix_ng");
PRINT_XMLHELP("unix_ng");

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_6( is_enabled, nullok, debug, quiet, use_first_pass, try_first_pass );
DECLARE_STRING_OPTS_1( authtok_type );
DECLARE_OPT_SETS;

module_helptext_t unix_ng_help[] = {{"", NULL, "Use pam_unix_ng.so as standard UNIX PAM module."},
				    {"debug", NULL, "Add <option>debug</option> option to all pam_unix_ng.so invocations."},
				    {"nullok", NULL, "Add <option>nullok</option> option to all pam_unix_ng.so invocations."},
				    {"quiet", NULL, "Add <option>quiet</option> option to pam_unix_ng.so."},
				    {NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_unix_ng = { "pam_unix_ng.so", opt_sets, unix_ng_help,
				 &def_parse_config,
				 &def_print_module,
				 &write_config_unix_ng,
				 &get_opt_set,
				 &getopt,
				 &print_args,
				 &print_xmlhelp,
				 PAM_UNIX_NG_AUTH_PRIORITY,
				 PAM_UNIX_NG_ACCOUNT_PRIORITY,
				 PAM_UNIX_NG_PASSWORD_PRIORITY,
				 PAM_UNIX_NG_SESSION_PRIORITY
};
