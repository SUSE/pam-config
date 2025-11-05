/* Copyright (C) 2023 SUSE LLC
   Author: William Brown <william@blackhats.net.au>

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
write_config_kanidm (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case AUTH:
      fprintf (fp, "auth\trequired\tpam_kanidm.so");
      break;
    case ACCOUNT:
      fprintf (fp, "account\trequired\tpam_kanidm.so");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_kanidm.so");
      break;
    case PASSWORD:
      fprintf (fp, "password\tsufficient\tpam_kanidm.so ");
      break;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("kanidm")
PRINT_XMLHELP("kanidm")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_kanidm = { "pam_kanidm.so", opt_sets, helptext,
                 &def_parse_config,
                 &def_print_module,
                 &write_config_kanidm,
                 &get_opt_set,
                 &getopt,
                 &print_args,
                 &print_xmlhelp,
		 NULL,
		 PAM_KANIDM_AUTH_PRIORITY,
		 PAM_KANIDM_ACCOUNT_PRIORITY,
		 -1,
		 PAM_KANIDM_SESSION_PRIORITY
};
