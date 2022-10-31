/* Copyright (C) 2020 Thorsten Kukuk
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
write_config_pwquality (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  /* pam_pwquality is only used in password part.  */
  if (op != PASSWORD)
    return 0;

  /* pam_pwquality is not enabled.  */
  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  fprintf (fp, "password\trequisite\tpam_pwquality.so\t");

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(PASSWORD)
GETOPT_END_1(PASSWORD)

PRINT_ARGS("pwquality")
PRINT_XMLHELP("pwquality")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_7(is_enabled, debug, reject_username, gecoscheck, enforce_for_root, local_users_only, use_authtok);
DECLARE_STRING_OPTS_17(authtok_type, retry, difok, minlen, dcredit, ucredit, lcredit, ocredit, minclass, dictpath, maxrepeat, maxsequence, maxclassrepeat, dictcheck, usercheck, enforcing, badwords);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_pwquality = { "pam_pwquality.so", opt_sets, helptext,
				  &def_parse_config,
				  &def_print_module,
				  &write_config_pwquality,
				  &get_opt_set,
				  &getopt,
				  &print_args,
				  &print_xmlhelp};
