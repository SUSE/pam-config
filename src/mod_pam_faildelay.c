/* Copyright (C) 2022 SUSE Linux GmbH
   Author: LEFEBVRE Valentin <valentin.lefebvre@suse.com>

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

#include <string.h>
#include <stdio.h>
#include "pam-module.h"
#include "pam-config.h"

static int
write_config_faildelay (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  // Only the auth module type is provided
  if (op != AUTH)
    return 0;

  fprintf (fp, "auth\toptional\tpam_faildelay.so ");
  WRITE_CONFIG_OPTIONS
  return 0;
}

GETOPT_START_1(AUTH)
GETOPT_END_1(AUTH)

PRINT_ARGS("faildelay");
PRINT_XMLHELP("faildelay");

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_2(is_enabled, debug);
DECLARE_STRING_OPTS_1(delay);
DECLARE_OPT_SETS;

static module_helptext_t faildelay_helptext[] = {
    {"", NULL, "Enable/Disable pam_faildelay.so"},
    {NULL, NULL, NULL}
};

pam_module_t mod_pam_faildelay = { "pam_faildelay.so", opt_sets,
    faildelay_helptext,
    &def_parse_config,
    &def_print_module,
    &write_config_faildelay,
    &get_opt_set,
    &getopt,
    &print_args,
    &print_xmlhelp,
    NULL,
    PAM_FAILDELAY_AUTH_PRIORITY,
    -1,
    -1,
    -1
};

