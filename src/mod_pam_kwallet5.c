/* Copyright (C) 2017 Fabian Vogt
   Copyright (C) 2008 Vincent Untz
   Author: Fabian Vogt <fvogt@suse.com>

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
write_config_kwallet5 (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case AUTH:
      fprintf (fp, "auth\toptional\tpam_kwallet5.so\n");
      return 0;
      break;
    case ACCOUNT:
      return 0;
      break;
    case PASSWORD:
      /* use_authtok is not needed, it's used by default. */
      fprintf (fp, "password\toptional\tpam_kwallet5.so\n");
      return 0;
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_kwallet5.so\t");
      break;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("kwallet5");
PRINT_XMLHELP("kwallet5");

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_1(is_enabled);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t kwallet5_helptext[] = {{"", NULL, "Enable/Disable pam_kwallet5.so"},
                                                     {NULL, NULL, NULL}};


pam_module_t mod_pam_kwallet5 = { "pam_kwallet5.so", opt_sets,
				       kwallet5_helptext,
				       &def_parse_config,
				       &def_print_module,
				       &write_config_kwallet5,
				       &get_opt_set,
				       &getopt,
				       &print_args,
				       &print_xmlhelp,
				       PAM_KWALLET5_AUTH_PRIORITY,
				       -1,
				       PAM_KWALLET5_PASSWORD_PRIORITY,
				       PAM_KWALLET5_SESSION_PRIORITY
};
