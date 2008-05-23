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
write_config_limits (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case SESSION:
      fprintf (fp, "session\trequired\tpam_limits.so\t");
      break;
    default:
      return 0;
    }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_1(SESSION)
GETOPT_END_1(SESSION)

PRINT_ARGS("limits")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_4 (is_enabled, debug, change_uid, utmp_early);
DECLARE_STRING_OPTS_1 (conf);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_limits = {"pam_limits.so", opt_sets,
			       &def_parse_config,
			       &def_print_module,
			       &write_config_limits,
			       &get_opt_set,
			       &getopt,
			       &print_args};
