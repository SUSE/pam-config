/* Copyright (C) 2007, 2008 Thorsten Kukuk
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
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
write_config_localuser (pam_module_t *this, enum write_type op,
			FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_ldap, with_nam, with_winbind;

  if (debug)
    printf ("**** write_config_localuser (...)\n");

  if (op != ACCOUNT)
    return 0;

  /* pam_localuser is not enabled.  */
  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_ldap = is_module_enabled (common_module_list,
                                 "pam_ldap.so", op);
  with_nam = is_module_enabled (common_module_list,
                                "pam_nam.so", op);
  with_winbind = is_module_enabled (common_module_list,
                                    "pam_winbind.so", op);

  if (with_ldap || with_nam || with_winbind)
    fprintf (fp, "account\tsufficient\tpam_localuser.so ");
  else
    fprintf (fp, "account\trequired\tpam_localuser.so ");

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(ACCOUNT)
GETOPT_END_1(ACCOUNT)

PRINT_ARGS("localuser")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2 (is_enabled, debug);
DECLARE_STRING_OPTS_1 (file);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_localuser = { "pam_localuser.so", opt_sets,
				   &def_parse_config,
				   &def_print_module,
				   &write_config_localuser,
				   &get_opt_set,
				   &getopt,
				   &print_args};
