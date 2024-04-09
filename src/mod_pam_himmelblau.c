/* Copyright (C) 2023 SUSE LLC
   Author: David Mulder <dmulder@suse.com>

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
write_config_himmelblau (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_winbind, with_ldap, with_sss;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_winbind = is_module_enabled(common_module_list, "pam_winbind.so", op);
  with_ldap = is_module_enabled(common_module_list, "pam_ldap.so", op);
  with_sss = is_module_enabled(common_module_list, "pam_sss.so", op);

  switch (op)
    {
    case ACCOUNT:
      if (with_winbind || with_ldap || with_sss) {
        fprintf (fp, "account\tsufficient\tpam_himmelblau.so\tuse_first_pass\t");
      } else {
        fprintf (fp, "account\trequired\tpam_himmelblau.so\tuse_first_pass\t");
      }
      break;
    case AUTH:
      if (with_winbind || with_ldap || with_sss) {
        fprintf (fp, "auth\tsufficient\tpam_himmelblau.so\tuse_first_pass\t");
      } else {
        fprintf (fp, "auth\trequired\tpam_himmelblau.so\tuse_first_pass\t");
      }
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_himmelblau.so\t");
      break;
    case PASSWORD:
      break;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("himmelblau")
PRINT_XMLHELP("himmelblau")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_himmelblau = { "pam_himmelblau.so", opt_sets, helptext,
				 &def_parse_config,
				 &def_print_module,
				 &write_config_himmelblau,
				 &get_opt_set,
				 &getopt,
				 &print_args,
				 &print_xmlhelp};
