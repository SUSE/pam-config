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
write_config_nam (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);

  if (debug)
    printf ("**** write_config_nam (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  switch (op)
    {
    case ACCOUNT:
      fprintf (fp, "account\trequired\tpam_nam.so\tuse_first_pass\n");
      break;
    case AUTH:
      fprintf (fp, "auth\trequired\tpam_nam.so\tuse_first_pass\n");
    case PASSWORD:
      fprintf (fp, "password\trequired\tpam_nam.so\ttry_first_pass\n");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_nam.so\n");
      break;
    }

  return 0;
}

PRINT_ARGS("nam")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_1 (is_enabled);
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_nam = { "pam_nam.so", opt_sets,
			     &def_parse_config,
			     &def_print_module,
			     &write_config_nam,
			     &get_opt_set,
			     NULL,
			     &print_args
};
