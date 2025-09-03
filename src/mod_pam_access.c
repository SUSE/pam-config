/* Copyright (C) 2014 Thorsten Kukuk
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
write_config_access (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  if (op != ACCOUNT)
    return 0;

  fprintf (fp, "account\trequired\tpam_access.so\t");

  WRITE_CONFIG_OPTIONS

  return 0;
}


GETOPT_START_1(ACCOUNT)
GETOPT_END_1(ACCOUNT)

PRINT_ARGS("access")
PRINT_XMLHELP("access")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_4 (is_enabled, debug, nodefgroup, noaudit);
DECLARE_STRING_OPTS_3 (accessfile, fieldsep, listsep);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{"", NULL, "pam_access for account access rules"},
				       {NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_access = { "pam_access.so", opt_sets, helptext,
			      &def_parse_config,
			      &def_print_module,
			      &write_config_access,
			      &get_opt_set,
			      &getopt,
			      &print_args,
			      &print_xmlhelp,
			      -1,
			      PAM_ACCESS_ACCOUNT_PRIORITY,
			      -1,
			      -1
};
